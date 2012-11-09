/**
  klasa przechowująca dane użytkownika
**/
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

class cUser
{
public:
   // user();
    cUser(cUser& u);                                    //konstruktor kopiujący
    cUser(cUser* u);                                    //konstruktor kopiujący

    cUser(int uid);                                     //konstruktor tworzący użytkownika o podanym uid z danych w bazie
    cUser(QString name);                                //konstruktor tworzący użytkownika o podanym nazwisku z danych w bazie

    cUser(QString name, QString mail, QString adress, bool male);
                                                        //konstruktor parametryczny wykorzystywany tylko przy tworzeniu
                                                        //nowego użytkownika, zawsze należy wywołać po nim funkcję initID
    void initID();                                      //inicjalizuje (z bazy) nr identyfikacyjny uzytkownika

    cUser(int uid, QString name, QString mail, QString adress, bool male, int nrOferty);
                                                        //konstruktor parametryczny inicjujący wszystkie pola
    ~cUser();

    QString name()const;                                //zwraca imię i nazwisko użytkownika - widoczne przy logowaniu i w tworzoych dokumentach
    QString mail()const;                                //zwraca adres email użytkownika
    QString adress()const;                              //zwraca adres bióra użytkownika w formacie htm
    bool male()const;                                   //zwraca: true - mężczyzna, false - kobieta
    QString uid()const;                                 //zwraca nr. identyfikacyjny użytkownika, na potrzeby zapisu
    QString dbName()const;                              //zwraca nazwę użytkownika MySQL - generowane na podstawie nazwiska
    int nrOferty()const;                                //zwraca aktualny (pierwszy nieużyty) numer oferty
    void nrOfertyInkrement();                           //inkrementuje nr oferty

private:
    QString* _name;
    QString* _mail;
    QString* _adress;
    bool _male;
    int _uid;
    int _nrOferty;
};



#endif // USER_H
