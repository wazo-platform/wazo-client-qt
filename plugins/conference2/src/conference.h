#ifndef __XLET_CONFERENCE
#define __XLET_CONFERENCE

#include <QDebug>
#include <QWidget>
#include <QtPlugin>
#include <QTabWidget>
#include <QVBoxLayout>

#include <xletinterface.h>
#include <xlet.h>

#include "conflist.h"

class XletConference : public XLet
{
    Q_OBJECT

    public:
        XletConference(QWidget *parent=0);

    private:
        QTabWidget *m_tab;
};

class XLetConferencePlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet *newXLetInstance(QWidget *parent=0);
};

#endif
