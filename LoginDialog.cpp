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
#include <QDir>
#include <QApplication>

#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include "User.h"
#include "Database.h"

LoginDialog::~LoginDialog()
{
    qDebug() <<  "destruktor Logowanie";

    delete ui;
    delete m_kOfertaLogo;
}

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    qDebug() << "Konstruktor Logowanie";

    ui->setupUi(this);

    m_kOfertaLogo = new QPixmap(":/klog");
    ui->img->setPixmap(*m_kOfertaLogo);

    m_db = new Database(this);

    QStringList nameFilter("*.ppk");
    QDir directory(qApp->applicationDirPath());
    QStringList userList = directory.entryList(nameFilter);
    ui->comboBox->addItems(userList);

    QSettings settings;
    settings.beginGroup("connection");
    if(settings.contains("last user"))
        ui->comboBox->setCurrentText(settings.value("last user").toString());
    settings.endGroup();

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QObject::connect(m_db, &Database::connectionFail, this, &LoginDialog::failed);
    QObject::connect(m_db, &Database::connectionSuccess, this, &LoginDialog::connectionSuccess);
    QObject::connect(m_db, &Database::connectionSuccess, this, &LoginDialog::accept);
    QObject::connect(m_db, &Database::changeStatus, ui->info, &QLabel::setText);
}

void LoginDialog::ok()
{
    ui->buttonBox->setEnabled(false);
    QString pass = ui->lineEdit->text();

    QSettings settings;
    settings.beginGroup("connection");
    settings.setValue("last user", ui->comboBox->currentText());
    settings.endGroup();

    m_db->setupDatabaseConnection(ui->comboBox->currentText(), pass);
}

void LoginDialog::failed()
{
    ui->buttonBox->setEnabled(true);
}
