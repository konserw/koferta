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
#include <QSqlRelationalTableModel>
#include <QSqlRecord>
#include "Macros.h"

SzukajOferty::SzukajOferty(QWidget *parent) :
  QWidget(parent),
    ui(new Ui::SzukajOferty)
{
    ui->setupUi(this);

    QString s;
    QSqlQuery q;
    QStringList sl;

    ui->label_wybierz->setText(tr("Wybierz ofertę do wczytania:"));
    ui->label_filtruj->setText(tr("Filtr:"));
    ui->dateEdit->setDisplayFormat("MMMM yy");
    ui->dateEdit->setDate(QDate::currentDate());

    s = "SELECT name FROM users";
    EXEC(s);
    while(q.next())
        sl << q.value(0).toString();
    ui->comboBox->insertItems(0, sl);

    sl.clear();
    sl << tr("Nr oferty") << tr("Klient") << tr("Data") << tr("Oferent");
    for(int i=0; i<sl.size(); ++i)
        ui->tabWidget->setTabText(i, sl[i]);

    model = new QSqlRelationalTableModel(this);
    model->setTable("zapisane");
    model->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
    model->setRelation(1, QSqlRelation("klient", "id", "short"));
    model->setRelation(3, QSqlRelation("users", "uid", "name"));
    model->select();

    for(int i=0; i<sl.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, sl[i]);

    ui->tableView->setModel(model);
    for(int i=4; i<model->columnCount(); ++i)
        ui->tableView->hideColumn(i);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->resizeColumnToContents(3);
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

void SzukajOferty::select(const QModelIndex& index)
{
    int row = index.row();
    QSqlRecord rec = model->record(row);
//    DEBUG << "selected row: " << row << "id: " << id;
    emit selectionChanged(rec);
}

void SzukajOferty::refId(const QString& id)
{
    QString s;
    s = "nr_oferty like '";
    s += id;
    s += "%'";
    model->setFilter(s);
}

void SzukajOferty::refClient(const QString& client)
{
    QString s;
    s = "short like '";
    s += client;
    s += "%'";
    model->setFilter(s);
}

void SzukajOferty::refDate(const QDate& date)
{
    QString sd = date.toString("MM.yyyy");
    DEBUG << "date: " << sd;
    QString s;
    s = "data like '%";
    s += sd;
    s += "'";
    model->setFilter(s);
}


void SzukajOferty::refUser(const QString& user)
{
    QString s;
    s = "name = '";
    s += user;
    s += "'";
    model->setFilter(s);
}
