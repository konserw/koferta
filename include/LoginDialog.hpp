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

#ifndef LOG_H
#define LOG_H

#include <QDialog>
#include <QHash>
#include "User.hpp"


namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    User user() const;

public slots:
    void openDBconnection();
    void ok();

private:
    Ui::LoginDialog *ui;

    QPixmap* m_kOfertaLogo;
    QString m_lastUser;
    QHash<QString, int> m_userList;
    User m_user;

    void readSettings();
    void writeSettings();
};
#endif // LOG_H
