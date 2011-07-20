#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "aastrasipnotify.h"
#include "incomingwidget.h"
#include "popcaastra.h"
#include "channelinfo.h"
#include "phoneinfo.h"
#include "userinfo.h"

IncomingWidget::IncomingWidget(int line, const QString & xchan, QWidget * w)
    : QWidget(w), m_line(line), m_xchannel(xchan),
        m_start(b_engine->timeServer())
{
    // qDebug() << Q_FUNC_INFO << "Line(" << line << ") Channel(" << xchan << ")";
    m_small_button_sz = new QSize(20,20);
    buildLayout();
    setSignalsSlots();
    updateFromChannelInfo(m_xchannel);
    refreshUI();
}

void IncomingWidget::buildLayout()
{
    // qDebug() << Q_FUNC_INFO;
    m_layout = new QHBoxLayout(this);
    m_lbl_line = new QLabel(this);
    m_lbl_name = new QLabel(this);
    m_lbl_exten = new QLabel(this);
    m_lbl_time = new QLabel(this);

    m_btn_hangup = new QPushButton("&H", this);
    m_btn_hold = new QPushButton("&h", this);
    m_btn_park = new QPushButton("&p", this);
    m_btn_atxfer = new QPushButton("&a", this);
    m_btn_xfer = new QPushButton("&t", this);
    m_btn_conf = new QPushButton("&c", this);

    m_btn_hangup->setMaximumSize(*m_small_button_sz);
    m_btn_hold->setMaximumSize(*m_small_button_sz);
    m_btn_park->setMaximumSize(*m_small_button_sz);
    m_btn_atxfer->setMaximumSize(*m_small_button_sz);
    m_btn_xfer->setMaximumSize(*m_small_button_sz);
    m_btn_conf->setMaximumSize(*m_small_button_sz);

    m_layout->addWidget(m_lbl_line, 0, Qt::AlignLeft);
    m_layout->addWidget(m_lbl_name, 0, Qt::AlignLeft);
    m_layout->addWidget(m_lbl_exten, 1, Qt::AlignLeft);
    m_layout->addWidget(m_btn_hangup, 0, Qt::AlignLeft);
    m_layout->addWidget(m_btn_hold, 0, Qt::AlignLeft);
    m_layout->addWidget(m_btn_atxfer, 0, Qt::AlignLeft);
    m_layout->addWidget(m_btn_xfer, 0, Qt::AlignLeft);
    m_layout->addWidget(m_btn_conf, 0, Qt::AlignLeft);
    m_layout->addWidget(m_btn_park, 0, Qt::AlignLeft);
    m_layout->addWidget(m_lbl_time, 0, Qt::AlignRight);
}

void IncomingWidget::refreshUI()
{
    // qDebug() << Q_FUNC_INFO;
    m_lbl_line->setText(QString("%1").arg(m_line));
    m_lbl_name->setText(m_peer_name);
    m_lbl_exten->setText(m_peer_number);
    m_lbl_time->setText(b_engine->timeElapsed(m_start));
    // TODO: Display hold / parked status
    updateCallTimeLabel();
}

void IncomingWidget::updateFromChannelInfo(const QString & xcid)
{
    // qDebug() << Q_FUNC_INFO << xcid;
    const ChannelInfo * info = b_engine->channel(xcid);
    if (! info) return;
    m_parkedCall = info->isparked();
    m_holdedCall = info->isholded();
    // TODO: When the caller is not a Xivo user
    m_peer = b_engine->getUserForXChannelId(info->talkingto_id());
    if (! m_peer) return;
    m_peer_name = m_peer->fullname();
    // TODO: Find something better than the first phone
    m_peer_number = b_engine->phone(m_peer->phonelist()[0])->number();
}

void IncomingWidget::setSignalsSlots()
{
    // qDebug() << Q_FUNC_INFO;
    connect(m_btn_atxfer, SIGNAL(clicked()), this, SLOT(doAttendedTransfer()));
    connect(m_btn_conf, SIGNAL(clicked()), this, SLOT(doConf()));
    connect(m_btn_hangup, SIGNAL(clicked()), this, SLOT(doHangUp()));
    connect(m_btn_hold, SIGNAL(clicked()), this, SLOT(doHold()));
    connect(m_btn_park, SIGNAL(clicked()), this, SLOT(doParkCall()));
    connect(m_btn_xfer, SIGNAL(clicked()), this, SLOT(doBlindTransfer()));
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
    // qDebug() << Q_FUNC_INFO;
    updateFromChannelInfo(m_xchannel);
    refreshUI();
}

void IncomingWidget::timerEvent(QTimerEvent *)
{
    updateCallTimeLabel();
}

void IncomingWidget::doHangUp()
{
    // qDebug() << Q_FUNC_INFO;
    emit doHangUp(m_line);
}

/*! \brief Put this line on hold
 *
 * Receives the click from the hold button and ask the POPC xlet to put that
 * line on hold */
void IncomingWidget::doHold()
{
    // qDebug() << Q_FUNC_INFO;
    emit doHold(m_line);
}

void IncomingWidget::doAttendedTransfer()
{
    // qDebug() << Q_FUNC_INFO;
    emit doAttendedTransfer(m_line);
}

/*! \brief Sends a blind transfer request to the POPC xlet */
void IncomingWidget::doBlindTransfer()
{
    // qDebug() << Q_FUNC_INFO;
    const ChannelInfo * c = b_engine->channel(m_xchannel);
    QString peer_channel = c->talkingto_id();
    QString peer_device = peer_channel.split("-").at(0);
    emit doBlindTransfer(peer_device, m_line, m_peer_name, m_peer_number);
}

void IncomingWidget::doConf()
{
    // qDebug() << Q_FUNC_INFO;
    emit doConf(m_line);
}

void IncomingWidget::doParkCall()
{
    // qDebug() << Q_FUNC_INFO;
    emit doParkCall(m_line);
}

void IncomingWidget::mousePressEvent(QMouseEvent * /* event */)
{
    // qDebug() << Q_FUNC_INFO << m_line;
    emit selectLine(m_line);
}

IncomingWidget::~IncomingWidget()
{
    // qDebug() << Q_FUNC_INFO;
    delete m_small_button_sz;
}
