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

#include "SzukajOferty.h"
#include "ui_SzukajOferty.h"
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QtDebug>
#include "Database.h"

SzukajOferty::SzukajOferty(QWidget *parent) :
  QWidget(parent),
    ui(new Ui::SzukajOferty)
{
    ui->setupUi(this);

    ui->label_wybierz->setText(tr("Wybierz ofertę do wczytania:"));
    ui->label_filtruj->setText(tr("Filtr:"));
    ui->dateEdit->setDisplayFormat("MMMM yy");
    ui->dateEdit->setDate(QDate::currentDate());

    Database db;
    ui->comboBox->insertItems(0, db.getUsersList());

    QStringList sl;
    sl << tr("Nr oferty") << tr("Klient") << tr("Data") << tr("Oferent");
    for(int i=0; i<sl.size(); ++i)
        ui->tabWidget->setTabText(i, sl[i]);

    model = new QSqlTableModel(this);
    model->setTable("savedOffersView");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->sortByColumn(2, Qt::DescendingOrder);

    connect(ui->tableView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(select(const QModelIndex&)));
    connect(ui->lineEdit_Id, SIGNAL(textEdited(QString)), this, SLOT(refId(const QString&)));
    connect(ui->lineEdit_Client, SIGNAL(textEdited(QString)), this, SLOT(refClient(const QString&)));
    connect(ui->dateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(refDate(const QDate&)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(refUser(const QString&)));
}

SzukajOferty::~SzukajOferty()
{
    delete ui;
}

QString SzukajOferty::currentOffer()
{
    QModelIndex idx = ui->tableView->currentIndex();
    return model->data(idx.sibling(idx.row(), 0)).toString();
}

void SzukajOferty::select(const QModelIndex& index)
{
    int row = index.row();
    QString offerId = model->data(index.sibling(row, 0)).toString();
    qDebug() << "selected row:" << row << "id:" << offerId;
    emit selectionChanged(offerId);
}

void SzukajOferty::refId(const QString& id)
{
    model->setFilter(QString("number like '%1%'").arg(id));
}

void SzukajOferty::refClient(const QString& client)
{
    model->setFilter(QString("customerCompany like '%1%' OR customerName like '%1%'").arg(client));
}

void SzukajOferty::refDate(const QDate& date)
{
    QString sd = date.toString("MM.yyyy");
    model->setFilter(QString("date >= str_to_date('1.%1', '%d.%m.%Y') AND  date <= str_to_date('31.%1', '%d.%m.%Y')").arg(sd));
}


void SzukajOferty::refUser(const QString& user)
{
    model->setFilter(QString("author = '%1'").arg(user));
}
