#include "marszruta.h"
#include "marszrutaPlugin.h"

#include <QtCore/QtPlugin>

marszrutaPlugin::marszrutaPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void marszrutaPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool marszrutaPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *marszrutaPlugin::createWidget(QWidget *parent)
{
    return new marszruta(parent);
}

QString marszrutaPlugin::name() const
{
    return QLatin1String("marszruta");
}

QString marszrutaPlugin::group() const
{
    return QLatin1String("");
}

QIcon marszrutaPlugin::icon() const
{
    return QIcon();
}

QString marszrutaPlugin::toolTip() const
{
    return QLatin1String("");
}

QString marszrutaPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool marszrutaPlugin::isContainer() const
{
    return false;
}

QString marszrutaPlugin::domXml() const
{
    return QLatin1String("<widget class=\"marszruta\" name=\"marszruta\">\n</widget>\n");
}

QString marszrutaPlugin::includeFile() const
{
    return QLatin1String("marszruta.h");
}

Q_EXPORT_PLUGIN2(marszrutaplugin, marszrutaPlugin)
