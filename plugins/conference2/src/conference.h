#ifndef __XLET_CONFERENCE
#define __XLET_CONFERENCE

#include <QDebug>
#include <QWidget>
#include <QtPlugin>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPushButton>

#include <xletinterface.h>
#include <xlet.h>

class XletConference;

#include "conflist.h"
#include "confchamber.h"

class ConfTab : public QTabWidget
{
    Q_OBJECT

    public:
        ConfTab(QWidget *parent);
        int addClosableTab(QWidget *w, const QString &title);
        void showConfRoom(const QString &id);

    private slots:
        void closeTab();
    private:
        QMap<QString, int> m_id2index;
};


class XletConference : public XLet
{
    Q_OBJECT

    public:
        XletConference(QWidget *parent=0);

    public slots:
        void openConfRoom(const QString &id);

    private:
        ConfTab *m_tab;
};

class XLetConferencePlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet *newXLetInstance(QWidget *parent=0);
};

#endif
