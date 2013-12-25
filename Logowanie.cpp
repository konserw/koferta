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
#include <QtWidgets>
#include <QPixmap>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QTextCodec>
#include <QInputDialog>
#include <QMessageBox>
#include "Logowanie.h"
#include "ui_Logowanie.h"
#include "User.h"
#include "Database.h"

Logowanie::~Logowanie()
{
    qDebug() <<  "destruktor Logowanie";

    delete ui;
    delete p;
    delete hosts;
}

Logowanie::Logowanie() :
    QDialog(nullptr),
    ui(new Ui::Logowanie)
{
    qDebug() << "Konstruktor Logowanie";

    ui->setupUi(this);

    p = new QPixmap(":/klog");
    ui->img->setPixmap(*p);

    hosts = new QStringList;

    QFile file("hosts");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "otworzono plik hosts";
        QTextStream in(&file);
        in.setCodec("UTF-8");
        QString s;
        do{
            s = in.readLine();
            if(!s.isEmpty())hosts->append(s);
        }while(!s.isNull());
        qDebug() << "hostów na liście: " << hosts->count();
    }
    else
        qWarning() << "otawrcie pliku hosts nie powiodło się";
    ui->ip->addItems(*hosts);

    m_db = new Database(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->add, SIGNAL(clicked()), this, SLOT(add()));
    connect(ui->ip, SIGNAL(currentIndexChanged(QString)), this, SLOT(hostChanged(QString)));

    QObject::connect(m_db, &Database::connectionSuccess, this, &Logowanie::connectionSuccess);
    QObject::connect(m_db, &Database::newUsers, this, &Logowanie::updateUserList);
    QObject::connect(m_db, &Database::changeStatus, ui->info, &QLabel::setText);
}

void Logowanie::hostChanged(QString ip)
{
    ui->comboBox->clear();
    m_db->hostChanged(ip);
}

void Logowanie::updateUserList(const QStringList& users)
{
    ui->comboBox->clear();
    ui->comboBox->addItems(users);
}


void Logowanie::add()
{
    QString s;
    bool ok;

    s = QInputDialog::getText(this, "Host", "Podaj adres IP hosta bazy danych MySql", QLineEdit::Normal, "", &ok);
    if(!ok) return;

    QFile file("hosts");
    if(file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        out.setCodec("UTF-8");

        out << "\n" << s;
        ui->ip->addItem(s);
    }
    else
    {
        QMessageBox::warning(this, "error!", "Plik hosts niedostepny.");
        qWarning() <<  "Plik host niedostepny.";
    }

}
void Logowanie::ok()
{
    QString pass = ui->lineEdit->text();
    if(pass.isEmpty())
    {
        qDebug() <<  "haslo nie wpisane";
        return;
    }

    m_db->connect(ui->comboBox->currentText(), pass);
}
