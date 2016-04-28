#include "Client.h"

Client::Client(const QString &ShortName, const QString &FullName, const QString &Title, const QString &Name, const QString &Surname, const QString &Address)
    : shortName(ShortName), fullName(FullName), title(Title), name(Name), surname(Surname), address(Address)
{

}

QString Client::concatedName() const
{
    return QString("%1 %2 %3").arg(title).arg(name).arg(surname);
}

QString Client::getAddress() const
{
    return address;
}

QString Client::getShortName() const
{
    return shortName;
}

QString Client::getFullName() const
{
    return fullName;
}
