/**
    kOferta - system usprawniajacy proces ofertowania
    Copyright (C) 2011  Kamil 'konserw' Strzempowicz, konserw@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "Logowanie.h"
#include "ui_Logowanie.h"

#include <QPixmap>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QTextCodec>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QInputDialog>
#include <QVariant>
#include <QMessageBox>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QProcess>

#ifndef NOSSL
    #ifdef WIN32
        #include <my_global.h>
        #include  <mysql.h>
    #else
       #include <mysql/mysql.h>
    #endif
#endif

#include "User.h"
#include "SHA1.h"
#include "Macros.h"
#include "NowyUser.h"

Logowanie::~Logowanie()
{
    DEBUG <<  "destruktor cLogowanie";
    delete p;
    delete d;
    delete ui;
    delete hosts;
}

Logowanie::Logowanie() :
    QDialog(NULL),
    ui(new Ui::Logowanie)
{
    DEBUG << "Konstruktor cLogowanie";

    ui->setupUi(this);

    p = new QPixmap(":/klog");
    ui->img->setPixmap(*p);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->add, SIGNAL(clicked()), this, SLOT(add()));
    connect(ui->ip, SIGNAL(currentIndexChanged(QString)), this, SLOT(hostChanged(QString)));

    d = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL"));
    d->setDatabaseName("kOferta");
    d->setPort(13306); 

#ifdef NOSSL
    DEBUG << "NOSSL defined, pomijam ustawianie bezpiecznego połączenia";
#else
    d->setConnectOptions("CLIENT_SSL=1;CLIENT_IGNORE_SPACE=1");

    QVariant v = d->driver()->handle();
    if (v.isValid() && qstrcmp(v.typeName(), "MYSQL*")==0)
    {
        MYSQL *handle = static_cast<MYSQL *>(v.data());
        if (handle != NULL)
        {
            mysql_ssl_set(handle, ":/client-key",
                ":/client-cert", ":/ca-cacert",
                NULL, NULL);
        }
    }
    else
    {
        QStringList list;
        QMessageBox::critical(this, tr("Błąd"), tr("Bład sterownika bazy danych!\nNastąpi zamknięcie programu."));
        DEBUG << "invalid driver";

        DEBUG << "library paths: ";
        list = qApp->libraryPaths();
        for(int i=0; i<list.size(); i++)
            DEBUG << "\t" << list[i];

        DEBUG << "aviable drivers: ";
        list = QSqlDatabase::drivers();
        for(int i=0; i<list.size(); i++)
            DEBUG << "\t" << list[i];
        this->reject();
        return;
    }
#endif
#ifdef RELEASE
    hosts = new QStringList;
    hosts->append("localhost");

    QFile file("host");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        DEBUG << "otworzono plik host";
        QTextStream in(&file);
        in.setCodec("UTF-8");
        QString s;
        do{
            s = in.readLine();
            if(!s.isEmpty())hosts->append(s);
        }while(!s.isNull());
        DEBUG << "hostów na liście: " << hosts->count();
    }
    else
        DEBUG << "otawrcie pliku host nie powiodło się";
    ui->ip->addItems(*hosts);
#else
    hosts = NULL;
    this->hostChanged(/*"konserw.no-ip.biz"*/"localhost");
    ui->comboBox->setCurrentIndex(5);
    ui->lineEdit->setFocus();
#endif
}

void Logowanie::hostChanged(QString ip)
{
    DEBUG << "Host został zmieniony na: " << ip;

    QString s;
    QSqlQuery q;

    ui->comboBox->clear();

    d->setHostName(ip);
    d->setUserName("kOferta_GetUsers");
    d->setPassword(GET_PASS);

    LOGIN_

    s = "SELECT name FROM users";
    EXEC(s);

    while(q.next())
         ui->comboBox->addItem(q.value(0).toString());
    ui->comboBox->addItem("Dodaj użytkownika");

    s = "SELECT DISTINCT ver, url FROM info";
    EXEC(s);
    q.next();
    ver = q.value(0).toDouble();
    if(ver > VER)
    {
        if(QMessageBox::Yes == QMessageBox::question(this, tr("Aktualizacja"), tr("Na serwerze jest dostępna nowa wersja programu kOferta.\n"
                                                                                  "Obecnie posiadasz wersję %1 a na serwerze dostępna jest wersja %2.\n"
                                                                                  "Pobrać ją teraz?").arg(QString::number(VER), QString::number(ver)), QMessageBox::Yes, QMessageBox::No))
        {
            DEBUG << "Wybrano aktualizacje";

            manager = new QNetworkAccessManager;
            connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));

            s = q.value(1).toString();
            QUrl url(s);
            DEBUG << "qurl: " << url.toString();
            QNetworkRequest req(url);
            manager->get(req);

            ui->info->setText(tr("Pobieranie aktualizacji w toku. Proszę czekać"));
            ui->ip->setEnabled(false);
            ui->buttonBox->setEnabled(false);
        }

        DEBUG << "koniec procedury sprawdzania wersji";

    }

    d->close();
}

void Logowanie::downloadFinished(QNetworkReply *reply)
{
    DEBUG << "pobieranie ukończone";

    if (reply->error())
    {
        QMessageBox::critical(this, tr("Błąd pobierania"), tr("Nastąpił błąd podczas pobierania aktualizacji. Prosimy spróbować aktualizacji w innym terminie lub skonsultować się z działem IT"), QMessageBox::Ok);

        DEBUG << "reply error: " << reply->errorString();

        ui->info->setText(tr("Aktualizacja nieudana!"));
        ui->ip->setEnabled(true);
        ui->buttonBox->setEnabled(true);
        reply->deleteLater();
        manager->deleteLater();
        return;
    }

    QString filename = "kOferta.7z";
  //  filename += QString::number(ver);
    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("Błąd otwarcia pliku"), tr("Nie udało się otworzyć pliku do zapisu! Prosimy spróbować aktualizacji w innym terminie lub skonsultować się z działem IT"));

        DEBUG << "Nie udane otwarcie pliku";

        ui->info->setText(tr("Aktualizacja nieudana!"));
        ui->ip->setEnabled(true);
        ui->buttonBox->setEnabled(true);
        reply->deleteLater();
        manager->deleteLater();
        return;
    }

    file.write(reply->readAll());
    file.close();

    reply->deleteLater();
    manager->deleteLater();
    //sprawdzanie sumy kontrolnej!
    QMessageBox::information(this, tr("Koniec pobierania!"), tr("Pobieranie aktualizacji zakończone powodzeniem. Nastąpi zakończenie programu i instalacja wersji %1").arg(QString::number(ver)));
    QProcess::startDetached("update.bat");
    qApp->quit();
}


void Logowanie::add()
{
    QString s;
    bool ok;

    s = QInputDialog::getText(this, "Host", "Podaj adres IP hosta bazy danych MySql", QLineEdit::Normal, "", &ok);
    if(!ok) return;

    QFile file("host");
    if(file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        out.setCodec("UTF-8");

        out << "\n" << s;
        ui->ip->addItem(s);
    }
    else
    {
        QMessageBox::warning(this, "error!", "Plik host niedostepny.");
        DEBUG <<  "Plik host niedostepny.";
    }

}
void Logowanie::ok()
{
    QString name;
    name = ui->comboBox->currentText();
    DEBUG << "Logowanie jako " << name;

    if(ui->lineEdit->text().isEmpty())
    {
        DEBUG <<  "hasło nie wpisane";
        return;
    }

    else if(name == "Dodaj użytkownika")
    {
        if(hash(ui->lineEdit->text()) != "810272f60e2d59adaa4b98b0546b2cebd7018b5c")
        {
            ui->info->setText("Błędne hasło!");
            ui->lineEdit->clear();
            return;
        }
        ui->lineEdit->clear();
        d->setUserName("kOferta_GetUsers");
        d->setPassword(GET_PASS);

        LOGIN

        NowyUser* nu = new NowyUser(this);
        nu->exec();
        delete nu;

        d->close();
        ui->ip->setCurrentIndex(ui->ip->currentIndex());
        //this->hostChanged(*host);
        return;
    }

    delete currentUser;
    currentUser = new cUser(name);

    d->setUserName(currentUser->dbName());
    d->setPassword(hash(ui->lineEdit->text()));

    LOGIN

    this->accept();
}
