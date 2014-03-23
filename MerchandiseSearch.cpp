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

#include <QSqlTableModel>
#include <QSqlRecord>
#include <QTableView>
#include <QHeaderView>
#include "MerchandiseSearchModel.h"
#include "MerchandiseSearch.h"
#include "ui_MerchandiseSearch.h"

MerchandiseSearch::MerchandiseSearch(MerchandiseSearchModel* model, QWidget *parent) :
    QDialog(parent),
    m_model(model),
    ui(new Ui::MerchandiseSearch)
{
    ui->setupUi(this);

    ui->label->setText(tr("Filtr:"));
    ui->radioButton_id->setText(tr("Id produktu"));
    ui->radioButton_id->setChecked(true);
    ui->radioButton_name->setText(tr("Nazwa produktu"));

    m_model->setFilter("");
    ui->tableView->setModel(m_model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);//:NoEditTriggers);

    connect(ui->radioButton_id, SIGNAL(clicked()), this, SLOT(ref2()));
    connect(ui->radioButton_name, SIGNAL(clicked()), this, SLOT(ref2()));
    connect(ui->lineEdit, SIGNAL(textEdited(QString)), this, SLOT(ref(QString)));
    connect(ui->pushButton_close, &QPushButton::clicked, this, &MerchandiseSearch::close);
}

MerchandiseSearch::~MerchandiseSearch()
{
    delete ui;
}

void MerchandiseSearch::ref2()
{
    ref(ui->lineEdit->text());
}

void MerchandiseSearch::ref(const QString & in)
{
    static const QString filter("%1 like '%2%'");
    m_model->setFilter(filter.arg(ui->radioButton_id->isChecked() ? "code" : "kOferta.merchandise.desc").arg(in));
}
