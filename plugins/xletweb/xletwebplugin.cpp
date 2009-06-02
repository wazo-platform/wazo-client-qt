#include <QDebug>
#include <QtPlugin>

#include "xletweb.h"
#include "xletwebplugin.h"

Q_EXPORT_PLUGIN2(xletwebplugin, XLetWebPlugin);

/*! \brief Destructor */
XLetWebPlugin::~XLetWebPlugin()
{
    qDebug() << "XLetWebPlugin::~XLetWebPlugin()";
}

/*! \brief instanciate XletWeb
 *
 * \return a new instance of XletWeb */
XLet * XLetWebPlugin::newXLetInstance(BaseEngine * engine, QWidget * parent)
{
    return new XletWeb(engine, parent);
}

