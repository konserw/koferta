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

#include "NowyUser.h"
#include "ui_NowyUser.h"

#include <QSqlQuery>
#include "SHA1.h"
#include "User.h"
#include "Macros.h"

NowyUser::NowyUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NowyUser)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

NowyUser::~NowyUser()
{
    delete ui;
}


void NowyUser::ok()
{
    DEBUG <<  "dodawanie użytkownika " << ui->name->text();
    if(ui->pass->text().isEmpty())
    {
        DEBUG <<  "puste pole hasła";
        QMessageBox::warning(this, "puste hasło", "Koniecznie wpisz hasło!");
        return;
    }

    QString s;
    QSqlQuery q;

    cUser u(ui->name->text(),                                   //nazwisko i imie
            ui->mail->text(),                                   //e-mail
            ui->adress->toPlainText().replace("\n", "<br>\n"),  //adres, dodane znaczniki htm
            ui->male->isChecked()                               //plec
            );


    s = "SELECT uid FROM users WHERE name='";
    s += u.name();
    s += "'";
    EXEC(s);
    if(q.next())
    {
        DEBUG <<  "użytkownik " << u.name() << " istnieje";
        if(QMessageBox::question(this, "użytkownik istnieje", "użytkownik o podanym imieniu i nazwisku już istnieje.\nZastąpić go nowym?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        {
            DEBUG <<  "wybrano usunięcie starego";

            s = "DROP USER ";
            s += u.dbName();
            EXEC(s);
            s = "DELETE FROM users WHERE name='";
            s += u.name();
            s += "'";
            EXEC(s);
        }
        else
        {
            DEBUG <<  "anulowano";
            return;
        }
    }

    s = "INSERT INTO users (name, mail, adress, male, nrOferty) VALUES ('";
    s += u.name();
    s += "', '";
    s += u.mail();
    s += "', '";
    s += u.adress();
    s += "', ";
    if(u.male())
        s += "TRUE";
    else
        s += "FALSE";
    s += ", 0)";

    EXEC(s);

    u.initID();

    s = "CREATE USER '";
    s += u.dbName();
    s += "'@'%' IDENTIFIED BY '";
    s += hash(ui->pass->text());
    s += "'";

    EXEC(s);

    s = "GRANT SELECT, INSERT, UPDATE, DELETE ON kOferta.* TO '";
    s += u.dbName();
    s += "'";
#ifndef NOSSL
    s += " REQUIRE SSL";
#endif
    EXEC(s);

    QMessageBox::information(this, "Pomyślnie dodano użytkownika", "Proces dodawania użytkownika zakończył się powodzeniem.");
    DEBUG <<  "dodano użytkownika " << u.name();
    this->accept();
    return;
 /*
    QMessageBox::warning(this, "błąd", "Wystąpił błąd");
    DEBUG <<  "błąd dodawania użytkownika";
*/
}
