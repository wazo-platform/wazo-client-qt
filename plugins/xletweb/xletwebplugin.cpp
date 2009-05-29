#include <QDebug>

#include "xletwebplugin.h"

Q_EXPORT_PLUGIN2(xletwebplugin, XletWebPlugin);

XLetWebPlugin::~XLetWebPlugin()
{
    qDebug() << "XLetWebPlugin::~XLetWebPlugin()";
}

XLet * XLetWebPlugin::newXLetInstance(BaseEngine * engine, QWidget * parent)
{
    return new XletWeb(engine, parent);
}

