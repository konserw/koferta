#include "TermItem.h"

TermItem::TermItem()
{
}

TermItem::TermItem(int id, QString shortDesc, QString longDesc) :
    m_id(id),
    m_shortDesc(shortDesc),
    m_longDesc(longDesc)
{
}

QString TermItem::longDesc() const
{
    return m_longDesc;
}

QString TermItem::shortDesc() const
{
    return m_shortDesc;
}
int TermItem::id() const
{
    return m_id;
}
