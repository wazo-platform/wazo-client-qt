#include "incomingwidget.h"

#include "baseengine.h"
#include "channelinfo.h"
#include "phoneinfo.h"

#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>

IncomingWidget::IncomingWidget(int line, const QString & channelxid,
                               const QString & phonexid, QWidget * w)
    : PendingWidget(phonexid, w), m_line(line), m_channel_xid(channelxid)
{
    buildui();
    update();
}

void IncomingWidget::buildui()
{
    this->PendingWidget::buildui(false);

    m_btn_answer = new QPushButton(tr("Answer"), this);
    m_btn_ignore = new QPushButton(tr("Ignore"), this);

    layout()->addWidget(m_btn_answer);
    layout()->addWidget(m_btn_ignore);

    connect(m_btn_ignore, SIGNAL(clicked()), this, SLOT(doIgnore()));
    connect(m_btn_answer, SIGNAL(clicked()), this, SLOT(doPickup()));

    connect(this, SIGNAL(ignore(int)), parent(), SLOT(hangUpLine(int)));
    connect(this, SIGNAL(pickup(int)), parent(), SLOT(selectLine(int)));
    connect(this, SIGNAL(remove_me(int)), parent(), SLOT(remove_incoming(int)));
}

void IncomingWidget::update()
{
    const ChannelInfo * c = b_engine->channel(m_channel_xid);
    QString display;
    if (c) {
        display = c->peerdisplay();
    } else {
        const PhoneInfo * p = b_engine->phone(phonexid());
        if (p) {
            display = p->number();
        } else {
            display = tr("Incoming call");
        }
    }
    set_string(QString("%0 %1").arg(display).arg(started_since()));
}

bool IncomingWidget::toRemove() const
{
    return false;
}

void IncomingWidget::doIgnore()
{
    emit ignore(m_line);
    emit remove_me(m_line);
}

void IncomingWidget::doPickup()
{
    emit pickup(m_line);
    emit remove_me(m_line);
}
