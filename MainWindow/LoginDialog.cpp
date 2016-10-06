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

    QStringList userList;
#ifdef RELEASE
    QStringList nameFilter("*.ppk");
    QDir directory(qApp->applicationDirPath());
    userList = directory.entryList(nameFilter);
#else
    userList
        << "kOf_Strzempowicz.ppk"
        << "kOf_Gubernat.ppk"
        << "kOf_Przybycien.ppk"
        << "kOf_Bauza.ppk"
        << "kOf_Baryla.ppk"
        << "kOf_Admin.ppk"
        << "kOf_Ciesielska.ppk";
#endif

    ui->comboBox->addItems(userList);

    QSettings settings;
    settings.beginGroup("connection");
    if(settings.contains("last user"))
        ui->comboBox->setCurrentText(settings.value("last user").toString());
    settings.endGroup();

    disconnect(this, SLOT(accept()));
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &LoginDialog::ok);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &LoginDialog::reject);
    connect(Database::instance(), &Database::driverFail, this, &LoginDialog::reject);
    connect(Database::instance(), &Database::connectionFail, this, &LoginDialog::failed);
    connect(Database::instance(), &Database::connectionSuccess, this, &LoginDialog::accept);
    connect(Database::instance(), &Database::changeStatus, ui->info, &QLabel::setText);
}

void LoginDialog::ok()
{
    ui->buttonBox->setEnabled(false);

    QSettings settings;
    settings.beginGroup("connection");
    settings.setValue("last user", ui->comboBox->currentText());
    settings.endGroup();

    Database::instance()->setupDatabaseConnection(ui->comboBox->currentText(), ui->lineEdit->text());
}

void LoginDialog::failed()
{
    ui->buttonBox->setEnabled(true);
}
