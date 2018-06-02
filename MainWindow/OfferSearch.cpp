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

#include "OfferSearch.h"
#include "ui_OfferSearch.h"
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QtDebug>
#include <QtWidgets>
#include "Database.h"

OfferSearch::OfferSearch(QWidget *parent) :
  QWidget(parent),
    ui(new Ui::OfferSearch)
{
    ui->setupUi(this);

    ui->dateEdit->setDisplayFormat("MMMM yyyy");
    ui->dateEdit->setDate(QDate::currentDate());
    ui->comboBox->addItem(tr("Wybierz użytkownika"));
    ui->comboBox->addItems(Database::instance()->usersList().keys());

    model = new QSqlTableModel(this);
    model->setTable("savedOffersView");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->sortByColumn(0, Qt::DescendingOrder);

    connect(ui->tableView, &QTableView::clicked, this, &OfferSearch::select);
    connect(ui->lineEdit_Id, &QLineEdit::textEdited, this, &OfferSearch::refSymbol);
    connect(ui->lineEdit_Client,  &QLineEdit::textEdited, this, &OfferSearch::refCustomer);
    connect(ui->dateEdit, &QDateEdit::dateChanged, this, &OfferSearch::refDate);
    //connect(ui->comboBox, &QComboBox::currentIndexChanged, this, &OfferSearch::refUser);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(refUser(const QString&)));
}

OfferSearch::~OfferSearch()
{
    delete ui;
}

int OfferSearch::offerIDfromIndex(const QModelIndex& index)
{
    return model->data(index.sibling(index.row(), 0)).toInt();
}

int OfferSearch::currentOffer()
{
    return offerIDfromIndex(ui->tableView->currentIndex());
}

void OfferSearch::select(const QModelIndex& index)
{
    int offerID = offerIDfromIndex(index);
    qDebug() << "selected row:" << index.row() << "id:" << offerID;
    emit selectionChanged(offerID);
}

void OfferSearch::refSymbol(const QString& symbol)
{
    model->setFilter(QString("offerSymbol like '%%%1%'").arg(symbol));
}

void OfferSearch::refCustomer(const QString& client)
{
    model->setFilter(QString("customerCompany like '%%%1%' OR customerName like '%%%1%'").arg(client));
}

void OfferSearch::refDate(const QDate& date)
{
    QString filter = QString("offerDate >= str_to_date('01.%1', '%d.%m.%Y') AND offerDate < str_to_date('01.%2', '%d.%m.%Y')")
                     .arg(date.toString("MM.yyyy"))
                     .arg(date.addMonths(1).toString("MM.yyyy"));
    qDebug() << "Date filter:" << filter;
    model->setFilter(filter);
}


void OfferSearch::refUser(const QString& user)
{
    model->setFilter(QString("author = '%1'").arg(user));
}
