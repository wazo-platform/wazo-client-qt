#include <QDebug>

#include "phoneinfo.h"
#include "popcaastra.h"
#include "transferedwidget.h"

class QHBoxLayout;

TransferedWidget::TransferedWidget(QString number, const QString & tname,
        const QString & tnum, QWidget * parent):
    QWidget(parent),
    m_number(number)
{
    qDebug() << Q_FUNC_INFO;

    m_time_transfer = b_engine->timeServer();

    m_layout = new QHBoxLayout(this);
    m_lbl_status = new QLabel(this);
    m_layout->addWidget(m_lbl_status);
    m_lbl_transfered_name = new QLabel(this);
    m_layout->addWidget(m_lbl_transfered_name);
    m_lbl_transfered_number = new QLabel(this);
    m_layout->addWidget(m_lbl_transfered_number);
    m_lbl_called_name = new QLabel(this);
    m_layout->addWidget(m_lbl_called_name);
    m_lbl_called_number = new QLabel(this);
    m_layout->addWidget(m_lbl_called_number);
    m_lbl_time = new QLabel(this);
    m_layout->addWidget(m_lbl_time);

    m_lbl_transfered_name->setText(tname);
    m_lbl_transfered_number->setText(tnum);

    findXphoneId();
    updateWidget();
}

/*! \brief refresh widgets */
void TransferedWidget::updateWidget()
{
    qDebug() << Q_FUNC_INFO;

    const PhoneInfo * called = b_engine->phone(m_called_phone_id);
    if (called == NULL) {
        qDebug() << Q_FUNC_INFO << "Cannot initialize widget, missing phoneinfo";
        return;
    }
    if (b_engine->getOptionsPhoneStatus().contains(called->hintstatus())) {
        QVariantMap s = b_engine->getOptionsPhoneStatus().value(called->hintstatus()).toMap();
        foreach (QString key, s.keys()) {
            qDebug() << Q_FUNC_INFO << key << ":" << s[key];
        }
        QString string = s.value("longname").toString();
        qDebug() << Q_FUNC_INFO << "Status: " << string;
        m_lbl_status->setText(
            b_engine->getOptionsPhoneStatus().value(called->hintstatus()).toMap()
                .value("longname").toString());
    } else {
        m_lbl_status->setText(tr("Unknown"));
    }
    m_lbl_called_name->setText(tr("Unknown"));
    foreach (QString user, b_engine->iterover("users").keys()) {
        if (b_engine->user(user)->phoneNumber() == m_number) {
            m_lbl_called_name->setText(b_engine->user(user)->fullname());
        }
    }
    m_lbl_called_number->setText(m_number);
    m_lbl_time->setText(b_engine->timeElapsed(m_time_transfer));
}

/*! \brief set m_called_phone_id */
void TransferedWidget::findXphoneId()
{
    qDebug() << Q_FUNC_INFO;
    foreach (QString xphoneid, b_engine->iterover("phones").keys()) {
        const PhoneInfo * phone = b_engine->phone(xphoneid);
        if (phone->number() == m_number) {
            m_called_phone_id = xphoneid;
            return;
        }
    }
}

