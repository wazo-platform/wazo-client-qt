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

IncomingWidget::IncomingWidget(int line, const QString & xchan, QWidget * w)
    : QWidget(w), m_line(line), m_xchannel(xchan),
        m_start(b_engine->timeServer())
{
    qDebug() << Q_FUNC_INFO << "Line(" << line << ") Channel(" << xchan << ")";
    buildLayout();
    setSignalsSlots();
    updateFromChannelInfo(m_xchannel);
    refreshUI();
}

void IncomingWidget::buildLayout()
{
    qDebug() << Q_FUNC_INFO;
    m_layout = new QHBoxLayout(this);
    m_lbl_line = new QLabel(this);
    m_lbl_name = new QLabel(this);
    m_lbl_exten = new QLabel(this);
    m_lbl_time = new QLabel(this);

    m_layout->addWidget(m_lbl_line);
    m_layout->setStretch(0,0);
    m_layout->addWidget(m_lbl_name);
    m_layout->setStretch(1,0);
    m_layout->addWidget(m_lbl_exten);
    m_layout->setStretch(2,1);
    m_layout->addWidget(m_lbl_time);
    m_layout->setStretch(3,0);

    m_hangUpAction = new QAction(tr("&Hangup"), this);
    m_attendedTransferAction = new QAction(tr("Attended transfer"), this);
    m_blindTransferAction = new QAction(tr("Blind transfer"), this);
    m_parkCallAction = new QAction(tr("Park call"), this);

    m_hangUpAction->setStatusTip(tr("Hang up/close the call"));
    m_blindTransferAction->setStatusTip(tr("Transfer the call"));
}

void IncomingWidget::refreshUI()
{
    qDebug() << Q_FUNC_INFO;
    m_lbl_line->setText(QString("%1").arg(m_line));
    m_lbl_name->setText(m_peer_name);
    m_lbl_exten->setText(m_peer_number);
    m_lbl_time->setText(b_engine->timeElapsed(m_start));
    // TODO: Display hold / parked status
    updateCallTimeLabel();
}

void IncomingWidget::updateFromChannelInfo(const QString & xcid)
{
    qDebug() << Q_FUNC_INFO << xcid;
    const ChannelInfo * info = b_engine->channel(xcid);
    if (! info) return;
    m_parkedCall = info->isparked();
    m_holdedCall = info->isholded();
    // TODO: When the caller is not a Xivo user
    m_peer = b_engine->getUserForXChannelId(info->talkingto_id());
    if (! m_peer) return;
    qDebug() << Q_FUNC_INFO << "My peer" << m_peer->toString();
    m_peer_name = m_peer->fullname();
    // TODO: Find something better than the first phone
    m_peer_number = b_engine->phone(m_peer->phonelist()[0])->number();
}

void IncomingWidget::setSignalsSlots()
{
    qDebug() << Q_FUNC_INFO;
    connect(m_hangUpAction, SIGNAL(triggered()), this, SLOT(doHangUp()));
    connect(m_attendedTransferAction, SIGNAL(triggered()), this,
                SLOT(doAttendedTransfer()));
    connect(m_blindTransferAction, SIGNAL(triggered()), this,
                SLOT(doBlindTransfer()));
    connect(m_parkCallAction, SIGNAL(triggered()), this, SLOT(doParkCall()));
}

/*! \brief Returns a string representation of an incoming call widget */
QString IncomingWidget::toString() const
{
    QString s;
    s += "XChannel(" + m_xchannel + ") ";
    s += "Peer name(" + m_peer_name + ") ";
    s += "Peer number(" + m_peer_number + ") ";
    s += "Line(" + m_lbl_line->text() + ") ";
    return s;
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

void IncomingWidget::updateWidget()
{
    qDebug() << Q_FUNC_INFO;
    updateFromChannelInfo(m_xchannel);
    refreshUI();
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
    emit doBlindTransfer(m_line, m_peer_name, m_peer_number);
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

void IncomingWidget::mousePressEvent(QMouseEvent * /* event */)
{
    qDebug() << Q_FUNC_INFO << m_line;
    emit selectLine(m_line);
}

