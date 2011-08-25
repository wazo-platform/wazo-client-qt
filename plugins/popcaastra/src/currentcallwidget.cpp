#include "currentcallwidget.h"

#include "baseengine.h"

#include <QHBoxLayout>
#include <QPushButton>

class ChannelInfo;

CurrentCallWidget::CurrentCallWidget(const QString & phonexid,
                                     const QString & chanxid,
                                     int line, QWidget * parent)
    : PendingWidget(phonexid, parent), m_line(line), m_chan_xid(chanxid)
{
    buildui();
    update();
}

CurrentCallWidget::~CurrentCallWidget()
{ }

void CurrentCallWidget::update()
{
    static const QString s = "%0 %1";
    const ChannelInfo * c = b_engine->channel(m_chan_xid);
    QString display;
    if (c) {
        display = c->peerdisplay();
    } else {
        const PhoneInfo * p = b_engine->phone(phonexid());
        if (p) {
            display = p->number();
        }
    }
    set_string(QString(s).arg(display).arg(started_since()));
}

bool CurrentCallWidget::toRemove() const
{
    const ChannelInfo * c = b_engine->channel(m_chan_xid);
    if (! c || c->commstatus() != "linked-called") return true;
    return false;
}

void CurrentCallWidget::doAtxfer()
{
    emit atxfer();
}

void CurrentCallWidget::doConf()
{
    qDebug() << Q_FUNC_INFO;
}

void CurrentCallWidget::doHangup()
{
    emit hangup();
    emit remove_me();
}

void CurrentCallWidget::doHold()
{
    emit hold();
    emit remove_me();
}

void CurrentCallWidget::doPark()
{
    emit park();
    emit remove_me();
}

void CurrentCallWidget::doTxfer()
{
    emit txfer();
    emit remove_me();
}

void CurrentCallWidget::buildui()
{
    this->PendingWidget::buildui(false);

    m_btn_atxfer = new QPushButton(tr("Atxfer"), this);
    m_btn_txfer = new QPushButton(tr("Txfer"), this);
    m_btn_hold = new QPushButton(tr("Hold"), this);
    m_btn_hangup = new QPushButton(tr("Hang up"), this);
    m_btn_conf = new QPushButton(tr("Conference"), this);
    m_btn_park = new QPushButton(tr("Park"), this);

    layout()->addWidget(m_btn_atxfer);
    layout()->addWidget(m_btn_txfer);
    layout()->addWidget(m_btn_hold);
    layout()->addWidget(m_btn_hangup);
    layout()->addWidget(m_btn_conf);
    layout()->addWidget(m_btn_park);

    connect(m_btn_atxfer, SIGNAL(clicked()), this, SLOT(doAtxfer()));
    connect(m_btn_txfer, SIGNAL(clicked()), this, SLOT(doTxfer()));
    connect(m_btn_hold, SIGNAL(clicked()), this, SLOT(doHold()));
    connect(m_btn_hangup, SIGNAL(clicked()), this, SLOT(doHangup()));
    connect(m_btn_conf, SIGNAL(clicked()), this, SLOT(doConf()));
    connect(m_btn_park, SIGNAL(clicked()), this, SLOT(doPark()));

    connect(this, SIGNAL(atxfer()), parent(), SLOT(attendedTransfer()));
    connect(this, SIGNAL(hangup()), parent(), SLOT(hangup()));
    connect(this, SIGNAL(hold()), parent(), SLOT(hold()));
    connect(this, SIGNAL(park()), parent(), SLOT(park()));
    connect(this, SIGNAL(txfer()), parent(), SLOT(transfer()));

    connect(this, SIGNAL(remove_me()), parent(), SLOT(remove_current_call()));
}
