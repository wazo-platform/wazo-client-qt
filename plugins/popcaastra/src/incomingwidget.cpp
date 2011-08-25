#include "incomingwidget.h"

#include <QDebug>
#include <QPushButton>

IncomingWidget::IncomingWidget(int line, const QString & channelxid,
                               const QString & phonexid, QWidget * w)
    : PendingWidget(phonexid, w), m_line(line), m_channel_xid(channelxid)
{
    qDebug() << Q_FUNC_INFO << phonexid << line;
    buildui();
    update();
}

void IncomingWidget::buildui()
{
    qDebug() << Q_FUNC_INFO;
    this->PendingWidget::buildui(false);

    m_btn_answer = new QPushButton(tr("Answer"), this);
    m_btn_ignore = new QPushButton(tr("Ignore"), this);

    connect(m_btn_ignore, SIGNAL(clicked()), this, SLOT(doIgnore()));
    connect(m_btn_answer, SIGNAL(clicked()), this, SLOT(doPickup()));
}

void IncomingWidget::update()
{
    // const ChannelInfo * c = b_engine->channel(m_xchannel);
    // if (c) {
    //     m_parkedCall = c->isparked();
    //     m_holdedCall = c->isholded();
    //     const UserInfo * u = b_engine->getUserForXChannelId(m_xchannel);
    //     if (u) {
    //         m_peer_name = u->fullname();
    //         foreach (const QString & phonexid, u->phonelist()) {
    //             const PhoneInfo * p = b_engine->phone(phonexid);
    //             if (p && c->xid().contains(p->identity())) {
    //                 m_peer_number = p->number();
    //             }
    //         }
    //     }
    // }
    set_string("Info");
}

bool IncomingWidget::toRemove() const
{
    return false;
}

void IncomingWidget::doIgnore()
{
    qDebug() << Q_FUNC_INFO;
}

void IncomingWidget::doPickup()
{
    qDebug() << Q_FUNC_INFO;
}
