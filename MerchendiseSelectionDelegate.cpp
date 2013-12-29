#include "MerchendiseSelectionDelegate.h"

#include <QDoubleSpinBox>

MerchendiseSelectionDelegate::MerchendiseSelectionDelegate(QObject *parent) :
    QItemDelegate(parent)
{   
}

QWidget *MerchendiseSelectionDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
 //   emit itemCountChanged(model->data(index, Qt::UserRole).toInt(), count);
}

void MerchendiseSelectionDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

