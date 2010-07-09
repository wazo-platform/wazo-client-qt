#include "conference.h"

Q_EXPORT_PLUGIN2(xletconference2plugin, XLetConferencePlugin);

XLet* XLetConferencePlugin::newXLetInstance(QWidget *parent)
{
    return new XletConference(parent);
}

XletConference::XletConference(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Conference"));
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);
    m_tab = new QTabWidget(this);
    layout->addWidget(m_tab);
    m_tab->addTab(new ConfList(this), tr("Conference List"));
}
