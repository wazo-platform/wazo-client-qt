#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QDateTime>

#include "channelinfo.h"
#include "incomingwidget.h"
#include "userinfo.h"
#include "taintedpixmap.h"
#include "popcaastra.h"
#include "aastrasipnotify.h"

IncomingWidget::IncomingWidget(int line_num, const QString & xchannel, QWidget * parent)
    : QWidget(parent), m_line(line_num), m_xchannel(xchannel), m_image(16,16),
        m_start(b_engine->timeServer())
{
    qDebug() << Q_FUNC_INFO;
    m_layout = new QHBoxLayout(this);

    m_lbl_line = new QLabel(this);
    m_lbl_name = new QLabel(this);
    m_lbl_exten = new QLabel(this);
    m_lbl_time = new QLabel(this);
    m_lbl_direction = new QLabel(this);
    m_lbl_status = new QLabel(this);

    m_lbl_line->setText(QString("%1").arg(m_line));
    const ChannelInfo * channel = b_engine->channels()[m_xchannel];
    qDebug() << Q_FUNC_INFO << channel->talkingto_id();
    const UserInfo * peer;
    foreach (QString xuserid, b_engine->iterover("users").keys()) {
        const UserInfo * current = b_engine->user(xuserid);
        if (current->fullname() == channel->peerdisplay()) {
            peer = current;
            break;
        }
    }
    m_lbl_name->setText(channel->peerdisplay());
    m_lbl_exten->setText(peer->phoneNumber());
    m_lbl_time->setText(b_engine->timeElapsed(m_start));
    //m_lbl_status->setText("status");

    if (channel->direction() == "in") {
        qDebug() << Q_FUNC_INFO << " Direction in";
        m_lbl_direction->setPixmap(QPixmap(":/in_calls/leftarrow.png"));
    } else {
        qDebug() << Q_FUNC_INFO << " Direction out";
        m_lbl_direction->setPixmap(QPixmap(":/in_calls/rightarrow.png"));
    }
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
    m_hangUpAction->setStatusTip(tr("Hang up/close the call"));
    connect(m_hangUpAction, SIGNAL(triggered()),
            this, SLOT(doHangUp()));
    m_attendedTransferAction = new QAction(tr("Attended transfer"), this);
    connect(m_attendedTransferAction, SIGNAL(triggered()), this, SLOT(doAttendedTransfer()));
    m_blindTransferAction = new QAction(tr("Blind transfer"), this);
    m_blindTransferAction->setStatusTip(tr("Transfer the call"));
    connect(m_blindTransferAction, SIGNAL(triggered()), this, SLOT(doBlindTransfer()));
    m_parkCallAction = new QAction(tr("Park call"), this);
    connect(m_parkCallAction, SIGNAL(triggered()), this, SLOT(doParkCall()));
}

QString IncomingWidget::toString() const
{
    QString info = QString("Line: %1 ").arg(m_line);
    info.append(QString("Channel: %1 ").arg(m_xchannel));
    return info;
}

int IncomingWidget::line() const
{
    return m_line;
}

void IncomingWidget::updateWidget()
{
    const ChannelInfo * channelinfo = b_engine->channels().value(m_xchannel);
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
void IncomingWidget::setActionPixmap()
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
void IncomingWidget::updateCallTimeLabel()
{
    const ChannelInfo * channelinfo = b_engine->channels().value(m_xchannel);
    if (channelinfo == NULL)
        return;
    m_lbl_time->setText(b_engine->timeElapsed(m_start));
}

void IncomingWidget::timerEvent(QTimerEvent *)
{
    updateCallTimeLabel();
}

void IncomingWidget::doHangUp()
{
    qDebug() << Q_FUNC_INFO;
    emit doHangUp(1);
}

void IncomingWidget::doBlindTransfer()
{
    qDebug() << Q_FUNC_INFO;
    emit doBlindTransfer(m_line);
}

void IncomingWidget::doAttendedTransfer()
{
    qDebug() << Q_FUNC_INFO;
    emit doAttendedTransfer(m_line);
}

void IncomingWidget::doTransferToNumber(const QString & number)
{
    qDebug() << Q_FUNC_INFO << number;
}

void IncomingWidget::doParkCall()
{
    qDebug() << Q_FUNC_INFO;
    emit doParkCall(m_line);
}

void IncomingWidget::contextMenuEvent(QContextMenuEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    QMenu contextMenu;
    contextMenu.addAction(m_hangUpAction);
    contextMenu.addAction(m_attendedTransferAction);
    contextMenu.addAction(m_blindTransferAction);
    contextMenu.addAction(m_parkCallAction);
    contextMenu.exec(event->globalPos());
}

void IncomingWidget::mousePressEvent(QMouseEvent * event)
{
    qDebug() << Q_FUNC_INFO << m_line;
    emit selectLine(m_line);
}

