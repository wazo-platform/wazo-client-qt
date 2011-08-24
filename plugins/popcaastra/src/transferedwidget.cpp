#include "transferedwidget.h"

#include "baseengine.h"
#include "phoneinfo.h"

#include <QDebug>

TransferedWidget::TransferedWidget(const QString & phonexid,
                                   const QString & called_num,
                                   const QString & name,
                                   const QString & num,
                                   QWidget * parent)
    : PendingWidget(phonexid, parent), m_called_num(called_num),
     m_t_name(name), m_t_num(num)
{
    this->buildui();
    this->update();
}

void TransferedWidget::buildui()
{
    this->PendingWidget::buildui();
    connect(this, SIGNAL(pickup(const QString &)),
            parent(), SLOT(doIntercept(const QString &)));
}

void TransferedWidget::update()
{
    static QString base = QString("%0 (%1) %2 %3 %4 %5");
    static QString message = tr("transfered to");
    const PhoneInfo * p = b_engine->phone(phonexid());
    if (p) {
        const QStringList & channels = p->xchannels();
        const ChannelInfo * c = 0;
        if (channels.size() > 0) {
            const QString & chanxid = channels.last();
            c = b_engine->channel(chanxid);
        }
        QString status_string;
        if (b_engine->getOptionsPhoneStatus().contains(p->hintstatus())) {
            status_string = (b_engine->getOptionsPhoneStatus()
                             .value(p->hintstatus()).toMap()
                             .value("longname").toString());
        }
        QString s = (QString(base)
                     .arg(m_t_name)
                     .arg(m_t_num)
                     .arg(message)
                     .arg((c ? c->peerdisplay() : ""))
                     .arg(status_string)
                     .arg(started_since()));
        set_string(s);
    } else {
        qDebug() << "Cannot generate a string from an empty phone";
    }
}

bool TransferedWidget::toRemove() const
{
    const PhoneInfo * p = b_engine->phone(phonexid());
    if (! p || ! p->xchannels().size()) return true;
    const QString & chanxid = p->xchannels().last();

    // Matches my phone's id if it's too early to have the called's info
    const QStringList & my_identities = 
        b_engine->user(b_engine->getFullId())->identitylist();
    const ChannelInfo * c = b_engine->channel(chanxid);
    bool match = false;
    foreach (const QString & identity, my_identities) {
        if (c->talkingto_id().contains(identity)) {
            match = true;
            break;
        }
    }
    if (match) return false;

    if (c->commstatus() != "calling") return true;
    return false;
}

void TransferedWidget::doPickup()
{
    emit pickup(m_called_num);
    emit remove_me(id());
}
