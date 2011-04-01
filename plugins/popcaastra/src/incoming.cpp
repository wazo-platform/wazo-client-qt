#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>

#include "incoming.h"
#include "userinfo.h"

Incoming::Incoming(int line_num, UserInfo * u_info, const QString & xchannel, QWidget * parent)
    : QWidget(parent), m_line(line_num), m_uinfo(u_info), m_xchannel(xchannel)
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
    m_layout->addWidget(m_lbl_line);
    m_layout->addWidget(m_lbl_direction);
    m_layout->addWidget(m_lbl_name);
    m_layout->addWidget(m_lbl_exten);
    m_layout->addWidget(m_lbl_time);
    updateWidget();
}

void Incoming::updateWidget(const QString & xchannel)
{
    const ChannelInfo * const channelinfo = b_engine->channels().value(xchannel);
    if (channelinfo == NULL) {
        qDebug() << Q_FUNC_INFO << "Cannot find " << xchannel;
        return;
    }
    const QString status = channelinfo->commstatus();
    const QString direction = channelinfo->direction();
    m_parkedCall = channelinfo->isparked();
    m_holdedCall = channelinfo->isholded();

    setActionPixmap();
    updateCallTimeLabel();

    return;
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

