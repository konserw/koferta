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
#include <QString>
#include <QSettings>
#include <QDialogButtonBox>
#include "LoginDialog.hpp"
#include "ui_LoginDialog.h"
#include "Database.hpp"
#include "DatabaseHelpers.hpp"

LoginDialog::~LoginDialog()
{
    qDebug() <<  "destruktor Logowanie";

    delete ui;
    delete m_kOfertaLogo;
}

void LoginDialog::openDBconnection()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->comboBox_user->clear();
    writeSettings();

    QString schema;
    if(ui->checkBox_useTestDB->isChecked())
        schema = "kOferta_test";
    else
        schema = "kOferta_v4";

    try {
    Database::instance()->setupDatabaseConnection(ui->lineEdit_ip->text(), ui->lineEdit_port->text().toUInt(), schema, SQL_USER, SQL_PWD);
    } catch(const DatabaseException& e) {
        QMessageBox::critical(nullptr, QObject::tr("Błąd"), QObject::tr("Bład sterownika bazy danych!"));
        qFatal("%s", e.what());
        //Application will close
    }
}

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    qDebug() << "Konstruktor Logowanie";

    ui->setupUi(this);

    m_kOfertaLogo = new QPixmap(":/klog");
    ui->label_logo->setPixmap(*m_kOfertaLogo);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    disconnect(this, SLOT(accept()));
    connect(ui->pushButton_apply, &QPushButton::clicked, this, &LoginDialog::openDBconnection);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &LoginDialog::ok);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &LoginDialog::reject);
    connect(Database::instance(), &Database::connectionSuccess, this, &LoginDialog::connected);
    connect(Database::instance(), &Database::changeStatus, ui->label_statusBar, &QLabel::setText);

    readSettings();
    openDBconnection();
}

void LoginDialog::ok()
{
    QString user = ui->comboBox_user->currentText();
    int uid = m_userList.value(user);
    qDebug() << QString("Log in as %1 (%2)").arg(user).arg(uid);

    QSettings settings;
    settings.beginGroup("connection");
    settings.setValue("last user", user);
    settings.endGroup();

    m_user = Database::instance()->logIn(uid, ui->lineEdit_password->text());
    if(m_user.isValid())
    {
        qDebug() << "Logged in successfully";
        this->accept();
    }
    else
    {
        qDebug() << "Login failed";
        ui->label_statusBar->setText(tr("Logowanie nie powiodło się; sprawdź użytkownika i hasło."));
    }
}

void LoginDialog::connected()
{
    qDebug() << "Database connected successfully";
    m_userList = Database::instance()->usersList();
    ui->comboBox_user->addItems(m_userList.keys());
    if(!(m_lastUser.isEmpty() || m_lastUser.isNull()) && m_userList.contains(m_lastUser))
        ui->comboBox_user->setCurrentText(m_lastUser);

    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(true);
}

User LoginDialog::user() const
{
    return m_user;
}

void LoginDialog::readSettings()
{
    QSettings settings;
    settings.beginGroup("connection");
    if(settings.contains("last user"))
        m_lastUser = settings.value("last user").toString();
    ui->checkBox_useTestDB->setChecked(settings.value("testDB", false).toBool());
    ui->lineEdit_ip->setText(settings.value("DB_host", "127.0.0.1").toString());
    ui->lineEdit_port->setText(settings.value("DB_port", "13306").toString());
    settings.endGroup();
}

void LoginDialog::writeSettings()
{
    QSettings settings;
    settings.beginGroup("connection");
    settings.setValue("testDB", ui->checkBox_useTestDB->isChecked());
    settings.setValue("DB_host", ui->lineEdit_ip->text());
    settings.setValue("DB_port", ui->lineEdit_port->text());
    settings.endGroup();
}
