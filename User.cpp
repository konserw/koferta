/**
    kOferta - system usprawniajacy proces ofertowania
    Copyright (C) 2011  Kamil 'konserw' Strzempowicz, konserw@gmail.com

    this program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    this program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QStringList>
#include <QtDebug>
#include "User.h"

User::User(const User &u)
{
    _name = new QString(u.name());
    _mail = new QString(u.mail());
    m_phone = new QString(u.phone());
    _adress = new QString(u.address());
    _male = u._male;
    _nrOferty = u._nrOferty;
    _uid = u._uid;
}

User::User(int uid, QString name, QString phone, QString mail, QString adress, bool male, int nrOferty)
{
    qDebug() << "new user:" << uid << name << phone << mail << adress << male << nrOferty;

    _uid = uid;
    _name = new QString(name);
    _mail = new QString(mail);
    m_phone = new QString(phone);
    _adress = new QString(adress);
    _male = male;
    _nrOferty = nrOferty;
}

User::~User()
{
    delete _name;
    delete m_phone;
    delete _mail;
    delete _adress;
}

void User::nrOfertyInkrement()
{
    /********************
     * Też do bazy ?
     */

    _nrOferty++;

    QString s = QString("UPDATE user SET nrOferty=%1 WHERE uid=%2").arg(_nrOferty).arg(_uid);
    QSqlQuery q;

    if(q.exec(s) == false)
    {
        _nrOferty--;
        qCritical() << "Zapytanie mysql zkonczone niepowodzeniem!";
        qDebug() << "\tError text: " <<  q.lastError().text();
    }
}

QString User::address() const
{
   return *_adress;
}

QString User::mail() const
{
    return *_mail;
}

bool User::male() const
{
    return _male;
}

QString User::name() const
{
    return *_name;
}

bool User::hasPhone() const
{
    return !(m_phone->isEmpty());
}

QString User::phone() const
{
    return *m_phone;
}

int User::uid() const
{
    return _uid;
}

int User::nrOferty() const
{
    return _nrOferty;
}

QString User::dbName(const QString& name)
{
    QString s;
    s = name.split(' ').last();
    s.truncate(12);
    s.replace("ł", "l");
    s.replace("ą", "a");
    s.replace("ę", "e");
    s.replace("ó", "o");
    s.replace("ś", "s");
    s.replace("ż", "z");
    s.replace("ź", "z");
    s.replace("ć", "c");
    s.replace("ń", "n");
    s = "kOf_" + s;
    return s;
}
