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

#include "Logowanie.h"
#include "ui_Logowanie.h"
#include "User.h"
#include "Database.h"

Logowanie::~Logowanie()
{
    qDebug() <<  "destruktor Logowanie";

    delete ui;
    delete m_kOfertaLogo;
}

Logowanie::Logowanie() :
    QDialog(nullptr),
    ui(new Ui::Logowanie)
{
    qDebug() << "Konstruktor Logowanie";

    ui->setupUi(this);

    m_kOfertaLogo = new QPixmap(":/klog");
    ui->img->setPixmap(*m_kOfertaLogo);

    m_db = new Database(this);

    QSettings settings;
    settings.beginGroup("connection");

    if(settings.contains("last user"))
        m_lastUser = settings.value("last user").toString();

    settings.endGroup();

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QObject::connect(m_db, &Database::connectionSuccess, this, &Logowanie::connectionSuccess);
    QObject::connect(m_db, &Database::connectionSuccess, this, &Logowanie::accept);
    QObject::connect(m_db, &Database::newUsers, this, &Logowanie::updateUserList);
    QObject::connect(m_db, &Database::changeStatus, ui->info, &QLabel::setText);
    connect(this, &Logowanie::userListRequested, m_db, &Database::getUsersList);
//    m_db->setupInitialConnection();
    emit(userListRequested());
}

void Logowanie::updateUserList(const QStringList& users)
{
    ui->comboBox->clear();
    ui->comboBox->addItems(users);

    if(!m_lastUser.isEmpty())
        ui->comboBox->setCurrentText(m_lastUser);
}

void Logowanie::ok()
{
    QString pass = ui->lineEdit->text();
    if(pass.isEmpty())
    {
        qDebug() <<  "haslo nie wpisane";
        return;
    }

    QSettings settings;
    settings.beginGroup("connection");
    settings.setValue("last user", ui->comboBox->currentText());
    settings.endGroup();

    m_db->connect(ui->comboBox->currentText(), pass);
}
