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

#include "CustomerSearch.h"
#include "Customer.h"

#include <QSqlTableModel>
#include <QSqlRecord>
#include <QtWidgets>


CustomerSearch::CustomerSearch(QWidget *parent) :
  QWidget(parent)
{
    setupUi();

    model = new QSqlTableModel(this);
    model->setTable("klient");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    QStringList sl;
    sl << tr("Id") << tr("Nazwa") << tr("Nazwisko");
    for(int i=0; i<sl.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, sl[i]);

    tableView->setModel(model);
    tableView->hideColumn(0);
    for(int i=2; i<5; ++i)
        tableView->hideColumn(i);
    tableView->hideColumn(6);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->resizeColumnToContents(3);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(ref(const QString&)));
    connect(radioButton_nazwa, SIGNAL(clicked()), this, SLOT(ref2()));
    connect(radioButton_nazwisko, SIGNAL(clicked()), this, SLOT(ref2()));
    connect(tableView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(select(const QModelIndex&)));
}

CustomerSearch::~CustomerSearch()
{
}

void CustomerSearch::ref2()
{
    ref(lineEdit->text());
}

void CustomerSearch::select(const QModelIndex &idx)
{
    QSqlRecord rec = model->record(idx.row());
    //TODO: sprawdzic nazwy kolumn, moze zmienic w bazie; Do klasy Database to?
    //Customer* customer = new
    if(rec.isEmpty())
        emit selectionChanged(Customer());
    else
        emit selectionChanged(Customer(
                rec.value("short").toString(),
                rec.value("full").toString(),
                rec.value("tytul").toString(),
                rec.value("imie").toString(),
                rec.value("nazwisko").toString(),
                rec.value("adres").toString(),
                rec.value("id").toInt()
                ));
}

void CustomerSearch::ref(const QString& in)
{
    model->setFilter(QString("%1 like '%2%'")
                     .arg(radioButton_nazwa->isChecked() ? "short" : "nazwisko")
                     .arg(in)
                     );
}

void CustomerSearch::setupUi()
{
    if (objectName().isEmpty())
        setObjectName(QStringLiteral("CustomerSearch"));
    resize(301, 393);
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    radioButton_nazwa = new QRadioButton(this);
    radioButton_nazwa->setObjectName(QStringLiteral("radioButton_nazwa"));
    radioButton_nazwa->setChecked(true);

    verticalLayout->addWidget(radioButton_nazwa);

    radioButton_nazwisko = new QRadioButton(this);
    radioButton_nazwisko->setObjectName(QStringLiteral("radioButton_nazwisko"));

    verticalLayout->addWidget(radioButton_nazwisko);

    lineEdit = new QLineEdit(this);
    lineEdit->setObjectName(QStringLiteral("lineEdit"));

    verticalLayout->addWidget(lineEdit);

    tableView = new QTableView(this);
    tableView->setObjectName(QStringLiteral("tableView"));

    verticalLayout->addWidget(tableView);

    retranslateUi();
} // setupUi

void CustomerSearch::retranslateUi()
{
    setWindowTitle(tr("Customer Search"));
    radioButton_nazwa->setText(tr("Filtruj po nazwie"));
    radioButton_nazwisko->setText(tr("Filtruj po nazwisku"));
} // retranslateUi
