#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QDateTime>

#include "channelinfo.h"
#include "incoming.h"
#include "userinfo.h"
#include "taintedpixmap.h"
#include "popcaastra.h"
#include "aastrasipnotify.h"

Incoming::Incoming(int line_num, UserInfo * u_info, const QString & xchannel, QWidget * parent)
    : QWidget(parent), m_line(line_num), m_uinfo(u_info), m_xchannel(xchannel), m_image(16,16)
{
    qDebug() << Q_FUNC_INFO;
    m_layout = new QHBoxLayout(this);

    m_lbl_line = new QLabel(this);
    m_lbl_name = new QLabel(this);
    m_lbl_exten = new QLabel(this);
    m_lbl_time = new QLabel(this);
    m_lbl_direction = new QLabel(this);
    m_lbl_status = new QLabel(this);

    m_lbl_line->setText("1");
    m_lbl_name->setText("Name");
    m_lbl_exten->setText("5555");
    m_lbl_time->setText("0:00");
    m_lbl_status->setText("status");

    m_lbl_direction->setPixmap(QPixmap(":/in_calls/leftarrow.png"));
    m_layout->addWidget(m_lbl_line);
    m_layout->setStretch(0,0);
    m_layout->addWidget(m_lbl_direction);
    m_layout->setStretch(1,0);
    m_layout->addWidget(m_lbl_status);
    m_layout->setStretch(2,0);
    m_layout->addWidget(m_lbl_name);
    m_layout->setStretch(3,0);
    m_layout->addWidget(m_lbl_exten);
    m_layout->setStretch(4,1);
    m_layout->addWidget(m_lbl_time);
    m_layout->setStretch(5,0);
    updateWidget();

    m_hangUpAction = new QAction(tr("&Hangup"), this);
    m_hangUpAction->setStatusTip(tr("Hang up/close the channel"));
    connect(m_hangUpAction, SIGNAL(triggered()),
            this, SLOT(doHangUp()));
}

void Incoming::updateWidget()
{
    //const ChannelInfo * channelinfo = b_engine->channels().value(m_xchannel);
    // DEBUG
    ChannelInfo * channelinfo = new ChannelInfo("xivotest", "sip/1234");
    QVariantMap prop;
    prop["direction"] = "in";
    prop["commstatus"] = "ringing";
    prop["timestamp"] = b_engine->timeServer();
    channelinfo->updateStatus(prop);
    // END DEBUG
    if (channelinfo == NULL) {
        qDebug() << Q_FUNC_INFO << "Cannot find " << m_xchannel;
        return;
    }
    const QString status = channelinfo->commstatus();
    const QString direction = channelinfo->direction();
    m_parkedCall = channelinfo->isparked();
    m_holdedCall = channelinfo->isholded();

    setActionPixmap();
    updateCallTimeLabel();

    if (direction == "out")
        m_lbl_direction->setPixmap(QPixmap(":/in_calls/rightarrow.png"));
    else if (direction == "in")
        m_lbl_direction->setPixmap(QPixmap(":/in_calls/leftarrow.png"));
    else
        qDebug() << Q_FUNC_INFO << "unknown direction" << direction;

    // TODO: Status change depanding on time waited
    // TODO: Status change depanding on hold/park
}

/*! \brief set icon depending on status
 */
void Incoming::setActionPixmap()
{
    const ChannelInfo * channelinfo = b_engine->channels().value(m_xchannel);
    if (channelinfo == NULL)
        return;
    const QString status = channelinfo->commstatus();
    QString color;
    QString tooltip;
    if (b_engine->getOptionsPhoneStatus().contains(status)) {
        color = b_engine->getOptionsPhoneStatus().value(status).toMap().value("color").toString();
        tooltip = b_engine->getOptionsPhoneStatus().value(status).toMap().value("longname").toString();
    } else {
        color = "white";
        tooltip = "unknown status";
    }
    TaintedPixmap tp = TaintedPixmap(QString(":/images/phone-trans.png"), QColor(color));
    m_lbl_status->setPixmap(tp.getPixmap());
    setToolTip(tooltip);
}

/*! \brief update time displayed in m_lbl_time
 */
void Incoming::updateCallTimeLabel()
{
    const ChannelInfo * channelinfo = b_engine->channels().value(m_xchannel);
    if (channelinfo == NULL)
        return;
    m_lbl_time->setText(b_engine->timeElapsed(channelinfo->timestamp()));
}

void Incoming::timerEvent(QTimerEvent *)
{
    updateCallTimeLabel();
}

void Incoming::doHangUp()
{
    qDebug() << Q_FUNC_INFO;
    emit doHangUp(1);
}

void Incoming::doTransferToNumber(const QString & number)
{
    qDebug() << Q_FUNC_INFO << number;
}

void Incoming::doParkCall()
{
    qDebug() << Q_FUNC_INFO;
}

void Incoming::contextMenuEvent(QContextMenuEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    QMenu contextMenu;
    contextMenu.addAction(m_hangUpAction);
    contextMenu.exec(event->globalPos());
}

