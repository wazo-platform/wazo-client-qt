#include "conference.h"

Q_EXPORT_PLUGIN2(xletconference2plugin, XLetConferencePlugin);

ConfTab::ConfTab(QWidget *parent)
    : QTabWidget(parent)
{
}

void ConfTab::closeTab()
{
    int index = sender()->property("index").toInt();

    m_id2index.remove(m_id2index.key(index));
    removeTab(index);
}

int ConfTab::addClosableTab(QWidget *w, const QString &title)
{
    int index = addTab(w, title);

    QPushButton *p = new QPushButton("X");
    p->setFlat(true);
    p->setMaximumSize(12,20);
    p->setFocusPolicy(Qt::NoFocus);
    p->setProperty("index", index);
    connect(p, SIGNAL(clicked()), this, SLOT(closeTab()));
    tabBar()->setTabButton(index, QTabBar::RightSide, p);
    return index;
}

void ConfTab::showConfRoom(const QString &id)
{
    if (!m_id2index.contains(id)) {
        if (b_engine->eVM(QString("confrooms/%0/in").arg(id)).size() == 0)
            return ;

        QString roomName = b_engine->eV(
                                QString("confrooms/%0/name").arg(id)).toString();
        QString roomNumber = b_engine->eV(
                                QString("confrooms/%0/number").arg(id)).toString();

        int index = \
            addClosableTab(new ConfChamber(id),
                           QString("%0 (%1)").arg(roomName).arg(roomNumber));

        m_id2index.insert(id, index);
    }
    setCurrentIndex(m_id2index.value(id));
}

XLet* XLetConferencePlugin::newXLetInstance(QWidget *parent)
{
    return new XletConference(parent);
}

XletConference::XletConference(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Conference"));

    QVBoxLayout *vLayout = new QVBoxLayout();
    setLayout(vLayout);

    m_tab = new ConfTab(this);
    m_tab->addTab(new ConfList(this), tr("Conference room list"));
    vLayout->addWidget(m_tab);
}

void XletConference::openConfRoom(const QString &id)
{
    m_tab->showConfRoom(id);
}
