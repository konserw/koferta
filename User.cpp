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
#include <exception>
#include <QtDebug>
#include "User.h"

cUser::cUser(const cUser &u)
{
    _name = new QString(u.name());
    _mail = new QString(u.mail());
    _adress = new QString(u.adress());
    _male = u._male;
    _nrOferty = u._nrOferty;
    _uid = u._uid;
}

cUser::cUser(int uid, QString name, QString mail, QString adress, bool male, int nrOferty)
{
    qDebug() << "new user:" << uid << name << mail << adress << male << nrOferty;

    _uid = uid;
    _name = new QString(name);
    _mail = new QString(mail);
    _adress = new QString(adress);
    _male = male;
    _nrOferty = nrOferty;
}

cUser::~cUser()
{
    delete _name;
    delete _mail;
    delete _adress;
}

void cUser::nrOfertyInkrement()
{
    _nrOferty++;

    QString s = QString("UPDATE users SET nrOferty=%1 WHERE uid=%2").arg(_nrOferty).arg(_uid);
    QSqlQuery q;

    if(q.exec(s) == false)
    {
        _nrOferty--;
        qCritical() << "Zapytanie mysql zkonczone niepowodzeniem!";
        qDebug() << "\tError text: " <<  q.lastError().text();
   //     throw std::exception("Failed to execute query");
    }
}

QString cUser::adress() const
{
   return *_adress;
}

QString cUser::mail() const
{
    return *_mail;
}

bool cUser::male() const
{
    return _male;
}

QString cUser::name() const
{
    return *_name;
}

int cUser::uid() const
{
    return _uid;
}

int cUser::nrOferty() const
{
    return _nrOferty;
}

QString cUser::dbName(const QString& name)
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
