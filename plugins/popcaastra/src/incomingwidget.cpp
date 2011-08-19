#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
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
    updateFromChannelInfo();
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

void IncomingWidget::updateFromChannelInfo()
{
    const ChannelInfo * c = b_engine->channel(m_xchannel);
    if (c) {
        m_parkedCall = c->isparked();
        m_holdedCall = c->isholded();
        const UserInfo * u = b_engine->getUserForXChannelId(m_xchannel);
        if (u) {
            m_peer_name = u->fullname();
            foreach (const QString & phonexid, u->phonelist()) {
                const PhoneInfo * p = b_engine->phone(phonexid);
                if (p && c->xid().contains(p->identity())) {
                    m_peer_number = p->number();
                }
            }
        }
    }
}

void IncomingWidget::setSignalsSlots()
{
    connect(this, SIGNAL(doConf(int, const QString &)),
            parent(), SLOT(confLine(int, const QString &)));
    connect(this, SIGNAL(doHangUp(int)), parent(), SLOT(hangUpLine(int)));
    connect(this, SIGNAL(doHold(int)), parent(), SLOT(holdLine(int)));
    connect(this, SIGNAL(selectLine(int)), parent(), SLOT(selectLine(int)));
    connect(this, SIGNAL(doAttendedTransfer(int)),
            parent(), SLOT(attendedTransfer(int)));
    connect(this, SIGNAL(doBlindTransfer(
                             const QString &,
                             int, 
                             const QString &,
                             const QString &)),
            parent(), SLOT(blindTransfer(
                             const QString &,
                             int,
                             const QString &,
                             const QString &)));
    connect(this, SIGNAL(doParkCall(int, const QString &, const QString &)),
            parent(), SLOT(parkcall(int, const QString &, const QString &)));

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
    updateFromChannelInfo();
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
    QString peer_channel = c->xid();
    QString peer_device = QString("%0/%1")
        .arg(peer_channel.split("-").at(0));
    emit doBlindTransfer(peer_device, m_line, m_peer_name, m_peer_number);
}

void IncomingWidget::doConf()
{
    // qDebug() << Q_FUNC_INFO;
    QMap<QString, QPushButton *> meetme_map;
    QMessageBox box;
    foreach (const QString & mxid, b_engine->iterover("meetmes").keys()) {
        const MeetmeInfo * m = b_engine->meetme(mxid);
        if (! m) continue;
        meetme_map[m->xid()] = box.addButton(QString("%0 - %1")
                                             .arg(m->number()).arg(m->name()),
                                             QMessageBox::ActionRole);
        if (meetme_map.size() == 1) {
            box.setDefaultButton(meetme_map[m->xid()]);
        }
    }

    if (meetme_map.size() > 0) {
        box.setText(tr("Choose the conference room to transfer this call to"));
        box.addButton(QMessageBox::Cancel);
        box.exec();
        foreach (const QString & mxid, b_engine->iterover("meetmes").keys()) {
            const MeetmeInfo * m = b_engine->meetme(mxid);
            if (m && box.clickedButton() == meetme_map[m->xid()]) {
                return emit doConf(m_line, m->xid());
            }
        }
    } else {
        qDebug() << Q_FUNC_INFO << "No meetme available";
        return;
    }
    qDebug() << Q_FUNC_INFO << "No meetme selected";
}

/*! \brief Prompts the user for a parking */
void IncomingWidget::doParkCall()
{
    QStringList device_info = m_xchannel.split("/");
    const QString & device_identity = (QString("%0/%1")
                                       .arg(device_info.value(1))
                                       .arg(device_info.value(2)
                                            .split("-").value(0)));
    QMap<QString, QPushButton *> parking_map;
    QMessageBox box;
    foreach (const QString & pxid, b_engine->iterover("parkinglots").keys()) {
        const ParkingInfo * p = b_engine->parkinglot(pxid);
        if (! p) continue;
        parking_map[p->xid()] = box.addButton(QString("%0 - %1")
                                              .arg(p->number()).arg(p->name()),
                                              QMessageBox::ActionRole);
        if (parking_map.size() == 1) {
            box.setDefaultButton(parking_map[p->xid()]);
        }
    }

    if (parking_map.size() > 0) {
        box.setText(tr("Choose the parking to park the call to"));
        box.addButton(QMessageBox::Cancel);
        box.exec();
        foreach (const QString & pxid, b_engine->iterover("parkinglots").keys()) {
            const ParkingInfo * p = b_engine->parkinglot(pxid);
            if (p && box.clickedButton() == parking_map[p->xid()]) {
                return emit doParkCall(m_line, p->xid(), device_identity);
            }
        }
    } else {
        qDebug() << Q_FUNC_INFO << "No parking available";
        return;
    }
    qDebug() << Q_FUNC_INFO << "No parking selected";
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
