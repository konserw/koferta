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

#include <QString>
#include <QSqlQuery>
#include <QVariant>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "SHA1.h"
#include "User.h"
#include "Macros.h"

cUser::cUser(QString name)
{
    QSqlQuery q;
    QString s;

    DEBUG <<  "Pobieranie danych użytkownika " << name;

    QSqlDatabase* d = new QSqlDatabase(QSqlDatabase::database(QSqlDatabase::connectionNames().at(0), false));

    d->setUserName("kOf_GetUsers");
    d->setPassword(GET_PASS);

    LOGIN

    s = "SELECT DISTINCT uid, mail, adress, male, nrOferty  FROM users WHERE name='";
    s += name;
    s += "'";

    EXEC(s);

    q.next();

    _uid = q.value(0).toInt();
    _name = new QString(name);
    _mail = new QString(q.value(1).toString());
    _adress = new QString(q.value(2).toString());
    _male = q.value(3).toBool();
    _nrOferty = q.value(4).toInt();

    d->close();
}

cUser::cUser(int uid)
{
    QSqlQuery q;
    QString s;

    DEBUG << "Pobieranie danych użytkownika uid:" << uid;

    s = "SELECT DISTINCT name, mail, adress, male, nrOferty  FROM users WHERE uid=";
    s += QString::number(uid);
    EXEC(s);
    q.next();

    _uid = uid;
    _name = new QString(q.value(0).toString());
    _mail = new QString(q.value(1).toString());
    _adress = new QString(q.value(2).toString());
    _male = q.value(3).toBool();
    _nrOferty = q.value(4).toInt();
}

void cUser::initID()
{
    QString s;
    QSqlQuery q;

    s = "SELECT DISTINCT uid FROM users WHERE name='";
    s += _name;
    s += "'";
    EXEC(s);

    q.next();

    _uid = q.value(0).toInt();
    _nrOferty = (_uid+1)*1000;

    s = "UPDATE users SET nrOferty=";
    s += QString::number(_nrOferty);
    s += " WHERE uid=";
    s += QString::number(_uid);
    EXEC(s);

}

void cUser::nrOfertyInkrement()
{
    QString s;
    QSqlQuery q;

    _nrOferty++;

    s = "UPDATE users SET nrOferty=";
    s += QString::number(_nrOferty);
    s += " WHERE uid=";
    s += QString::number(_uid);
    EXEC(s);
}

cUser::cUser(cUser &u)
{
    _name = new QString(u.name());
    _mail = new QString(u.mail());
    _adress = new QString(u.adress());
    _male = u._male;
    _nrOferty = u._nrOferty;
    _uid = u._uid;

}

cUser::cUser(cUser* u)
{
    _name = new QString(u->name());
    _mail = new QString(u->mail());
    _adress = new QString(u->adress());
    _male = u->_male;
    _nrOferty = u->_nrOferty;
    _uid = u->_uid;
}

cUser::cUser(QString name, QString mail, QString adress, bool male)
{
    _name = new QString(name);
    _mail = new QString(mail);
    _adress = new QString(adress);
    _male = male;
}

cUser::cUser(int uid, QString name, QString mail, QString adress, bool male, int nrOferty)
{
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

QString cUser::dbName() const
{
    QString s;
    s = _name->split(' ').last();
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
