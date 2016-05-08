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
#include "ui_MerchandiseSearch.h"
#include "MerchandiseSelection.h"
#include "MerchandiseSelectionModel.h"
#include "MerchendiseSelectionDelegate.h"

MerchandiseSelection::MerchandiseSelection(const QHash<int, double> &hash, QWidget *parent) :
    MerchandiseSearch(new MerchandiseSelectionModel(hash, parent), parent)
{
    m_merchandiseModel = new QSqlTableModel(this);
    m_merchandiseModel->setTable("merchandise");
    m_merchandiseModel->select();
    m_merchandiseModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    m_model->setSourceModel(m_merchandiseModel);
    MerchendiseSelectionDelegate* delegate = new MerchendiseSelectionDelegate(this);
    ui->tableView->setItemDelegate(delegate);

    connect(static_cast<MerchandiseSelectionModel*>(m_model), &MerchandiseSelectionModel::itemCountChanged, this, &MerchandiseSelection::itemCountChanged);

    this->setWindowFlags(Qt::Window);
}

MerchandiseSelection::~MerchandiseSelection()
{

}
