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

#include <QDoubleSpinBox>
#include "MerchendiseSelectionDelegate.hpp"

MerchendiseSelectionDelegate::MerchendiseSelectionDelegate(QObject *parent) :
    QItemDelegate(parent)
{   
}

QWidget *MerchendiseSelectionDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &index) const
{
    if(!index.isValid() || index.column() != 0)
        return new QWidget(parent);

    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setMinimum(0);
    editor->setMaximum(99999);
    editor->setSingleStep(1);
    return editor;
}

void MerchendiseSelectionDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(!index.isValid() || index.column() != 0)
        return;

    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(index.model()->data(index, Qt::EditRole).toDouble());
}

void MerchendiseSelectionDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(!index.isValid() || index.column() != 0)
        return;

    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double count = spinBox->value();
    model->setData(index, count, Qt::EditRole);
}

void MerchendiseSelectionDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}

