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

Q_DECLARE_METATYPE(QWidget*);

class ConfTab : public QTabWidget
{
    Q_OBJECT

    public:
        ConfTab(QWidget *parent);
        int addClosableTab(QWidget *w, const QString &title);
        void showConfRoom(const QString &id, bool force);
        int indexOf(QWidget *w) { return QTabWidget::indexOf(w); };
        int indexOf(const QString &id);

    public slots:
        void closeTab(QWidget *w=0);
};


class XletConference : public XLet
{
    Q_OBJECT

    public:
        XletConference(QWidget *parent=0);

    public slots:
        void openConfRoom(const QString &id, bool force=false);
    private slots:
        void checkJoiningPeople(const QString &room, DStoreEvent e);

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
