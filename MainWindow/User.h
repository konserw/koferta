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

#ifndef USER_H
#define USER_H

class QString;
class QSqlDatabase;
/*!
 * \brief Klasa przechowująca dane użytkownika
 */
class User
{
public:
    explicit User();
    ///konstruktor parametryczny inicjujący wszystkie pola
    User(int uid, QString name, QString phone, QString mail, QString address, bool male, int nrOferty);
    ///konstruktor kopiujący
    User(const User& u);

    ~User();

    ///zwraca imię i nazwisko użytkownika - widoczne przy logowaniu i w tworzoych dokumentach
    QString name()const;
    ///returns true if user has phone number
    bool hasPhone() const;
    ///zwraca nr telefonu
    QString phone()const;
    ///zwraca adres email użytkownika
    QString mail()const;
    ///zwraca adres bióra użytkownika w formacie htm
    QString address()const;
    ///zwraca: true - mężczyzna, false - kobieta
    bool male()const;
    ///zwraca nr. identyfikacyjny użytkownika, na potrzeby zapisu
    int uid()const;
    ///zwraca aktualny (pierwszy nieużyty) numer oferty
    int nrOferty()const;
    ///inkrementuje nr oferty
    bool nrOfertyInkrement();
    bool isValid() const;

private:
    QString* _name;
    QString* m_phone;
    QString* _mail;
    QString* _adress;
    bool _male;
    int _uid;
    int _nrOferty;
    bool _valid;
};



#endif // USER_H
