#include <QSize>
#include <QDoubleSpinBox>
#include <QModelIndex>

#include "MerchandiseListDelegate.h"
#include "MerchandiseListModel.h"

MerchandiseListDelegate::MerchandiseListDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *MerchandiseListDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex & index ) const
{
    const MerchandiseListModel* m = static_cast<const MerchandiseListModel*>(index.model());

    if(m->isIlosc(index))
    {
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setMinimum(0);
        editor->setMaximum(99999);
        editor->setSingleStep(1);
        return editor;
    }

    if(m->isRabat(index))
    {
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setMinimum(0);
        editor->setMaximum(100);
        editor->setSingleStep(0.1);
        return editor;
    }

    return new QWidget(parent);
}

void MerchandiseListDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const MerchandiseListModel* m = static_cast<const MerchandiseListModel*>(index.model());
    if(m->isRabat(index) || m->isIlosc(index))
    {
        QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor);
        spinBox->setValue(m->data(index, Qt::EditRole).toDouble());
    }
}

void MerchandiseListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    const MerchandiseListModel* m = static_cast<const MerchandiseListModel*>(index.model());
    if(m->isIlosc(index) || m->isRabat(index))
    {
        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->interpretText();
        model->setData(index, spinBox->value(), Qt::EditRole);
    }
}

void MerchandiseListDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

