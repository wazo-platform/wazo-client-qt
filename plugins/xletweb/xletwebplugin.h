#ifndef __XLETWEBPLUGIN_H__
#define __XLETWEBPLUGIN_H__

#include <QObject>
#include "xletinterface.h"

/*! \brief XLetInterface implementation
 *
 * entry point to instanciate XletWeb
 */
class XLetWebPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)
public:
    ~XLetWebPlugin();
    XLet * newXLetInstance(BaseEngine * engine = 0, QWidget * parent = 0);
};

#endif

