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

#include "WyborTowaru.h"
#include "ui_WyborTowaru.h"
#include "MainWindow.h"
#include <QSqlRecord>

WyborTowaru::WyborTowaru(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WyborTowaru)
{
    ui->setupUi(this);

    ui->pushButton->setText(tr("Ok"));
    ui->label_add->setText(tr("Dodaj:"));
    ui->plainTextEdit->setPlainText("m_info");
    ui->spinBox->setMinimum(0);
    ui->spinBox->setSingleStep(1);
    ui->spinBox->setMaximum(99999);
    ui->spinBox->setEnabled(false);
    rec = NULL;

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    ui->plainTextEdit->setFont(font);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->widget, SIGNAL(selectionChanged(const QSqlRecord&)), this, SLOT(refresh(const QSqlRecord&)));
    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(spin(int)));
}

WyborTowaru::~WyborTowaru()
{
    delete ui;
}

void WyborTowaru::refresh(const QSqlRecord& _rec)
{
    if(rec && *rec == _rec)
        return;

    delete rec;
    rec = new QSqlRecord(_rec);

    if(_rec.isEmpty() || rec->value(0).toString().isEmpty())
    {
        ui->plainTextEdit->setPlainText("m_info");
        ui->spinBox->setValue(0);
        ui->spinBox->setEnabled(false);
        return;
    }

    QString s;
    s = tr("Kod towaru:\t");
    s += rec->value(0).toString();
    s += tr("\n\nSpecyfikacja:\t");
    s += rec->value(1).toString();
    s += tr("\n\nCena katalogowa:\t");
    s += rec->value(2).toString();
    s += tr("\n\nJednostka:\t");
    s += rec->value(3).toString();
    ui->plainTextEdit->setPlainText(s);

    ui->spinBox->setEnabled(true);
    ui->spinBox->setValue(dynamic_cast<MainWindow*>(this->parent())->ileTowaru(rec->value(0).toString()));
}

void WyborTowaru::spin(int ile)
{
    if(ile != 0 && rec != NULL)
        emit countChanged(*rec, ile);
}
