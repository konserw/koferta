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

#ifndef TERMSCHOOSERDIALOG_H
#define TERMSCHOOSERDIALOG_H

#include <QDialog>
#include "TermItem.h"

class QAbstractTableModel;

namespace Ui {
class TermsChooserDialog;
}

class TermsChooserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TermsChooserDialog(QWidget *parent = 0) = delete;
    explicit TermsChooserDialog(QWidget *parent, TermType type);
    ~TermsChooserDialog();

signals:
    void termChoosen(const TermItem& term);

public slots:
    void selectionChanged(QModelIndex);
    void ok();

private:
    Ui::TermsChooserDialog *ui;
};

#endif // TERMSCHOOSERDIALOG_H
