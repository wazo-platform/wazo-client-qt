/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $Revision$
 * $Date$
 */

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHash>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include <QApplication>
#include <QMenu>
#include <QDebug>
#include <QFontMetrics>

#include "baseengine.h"
#include "peerwidget.h"
#include "xivoconsts.h"
#include "userinfo.h"
#include "phoneinfo.h"

/*! \brief Constructor
 */
PeerWidget::PeerWidget(UserInfo *ui)
    : BasePeerWidget(ui), m_user_status(NULL), m_agentlbl(NULL), m_mobilelbl(NULL)
{
    int fsize = 25;

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    setLayout(vLayout);
    vLayout->setSpacing(0);
    vLayout->setMargin(0);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setSpacing(0);
    QWidget *peer = new QWidget(this);
    vLayout->addWidget(peer);
    peer->setStyleSheet(".QWidget {"
                            "border-style: dotted;"
                            "border-left-width: 1px;"
                            "border-color: #000000;"
                        "}");

    QGridLayout *layout = new QGridLayout(peer);
    peer->setLayout(layout);
    layout->setMargin(3);
    layout->setSpacing(0);

    m_textlbl = new QLabel(peer);
    m_textlbl->setMinimumWidth(m_maxWidthWanted);
    setName(m_ui->fullname());

    if (!m_ui->ctilogin().isEmpty()) {
        m_user_status = new ChitchatButton(peer, &m_ui);
        m_user_status->setProperty("userid", ui->userid());
        m_user_status->setProperty("astid", ui->ipbxid());
        m_user_status->setIconSize(QSize(fsize, fsize));
        m_user_status->setFixedWidth(fsize);
        m_user_status->setFlat(true);
        m_user_status->setFocusPolicy(Qt::NoFocus);

        if (b_engine->enabledFunction("chitchat")) {
            connect(m_user_status, SIGNAL(pressed()),
                    ChitChatWindow::chitchat_instance, SLOT(writeMessageTo()));
        }
        hLayout->addWidget(m_user_status);
    } else {
        hLayout->addSpacing(fsize);
    }

    foreach (QString phone, ui->phonelist()) {
        m_lblphones[phone] = new QLabel(peer);
        m_lblphones[phone]->setAlignment(Qt::AlignCenter);
        m_lblphones[phone]->setMinimumSize(fsize, fsize);
        m_lblphones[phone]->setProperty("kind", "term");

        hLayout->addWidget(m_lblphones[phone]);
    }


    if (!m_ui->mobileNumber().isEmpty()) {
        m_mobilelbl = new QLabel(peer);
        m_mobilelbl->setPixmap(QPixmap(":/images/mobile-grey.png"));
        m_mobilelbl->setAlignment(Qt::AlignCenter);
        m_mobilelbl->setMinimumSize(fsize, fsize);
        m_mobilelbl->setProperty("kind", "mobile");
        setMobileState("grey");

        hLayout->addWidget(m_mobilelbl);
    }

    if (!ui->agentNumber().isEmpty()) {
        m_agentlbl = new QLabel(peer);
        m_agentlbl->setAlignment(Qt::AlignCenter);
        m_agentlbl->setMinimumSize(fsize, fsize);
        m_agentlbl->setToolTip(tr("Agent %1").arg(ui->agentNumber()));
        m_agentlbl->setProperty("kind", "agent");
        setAgentState("grey");

        hLayout->addWidget(m_agentlbl);
    }
    hLayout->addStretch(1);

    layout->addWidget(m_textlbl, 0, 2, 1, 1, Qt::AlignLeft);
    layout->addLayout(hLayout, 1, 2);
    setMaximumWidth(200);

    reloadSavedName();
}

void PeerWidget::setAgentState(const QString &color)
{
    if (m_agentlbl) {
        QColor c = QColor(color);
        m_agentlbl->setPixmap(TaintedPixmap( \
            QString(":/images/agent-trans.png"), c).getPixmap());
    }
}

void PeerWidget::setMobileState(const QString &/* color*/)
{
    if (m_mobilelbl) {
        m_mobilelbl->setToolTip(tr("Mobile number : %1").arg(m_ui->mobileNumber()));
    }
}

void PeerWidget::updatePresence()
{
    if (m_user_status) {
        QString availstate = m_ui->availstate();
        QColor c = QColor("green"); // XXX replace with dependency on m_ui->availstate()
        m_user_status->setIcon(TaintedPixmap( \
            QString(":/images/personal-trans.png"), c).getPixmap());
        m_user_status->setToolTip(tr("User : %1").arg("La")); // XXX replace with dependency on m_ui->availstate()
    }
}

void PeerWidget::updatePhonesStates()
{
    QString ipbxid = m_ui->ipbxid();
    foreach(QString phoneid, m_ui->phonelist()) {
        QString xphoneid = QString("%1/%2").arg(ipbxid).arg(phoneid);
        const PhoneInfo * pi = b_engine->phones().value(xphoneid);
        if (pi) {
            QString color = "white"; // XXXX function of p_pi->hintstatus();
            QString longname = "Here"; // XXXX function of p_pi->hintstatus();
            QColor c = QColor(color);
            m_lblphones[phoneid]->setPixmap( \
                TaintedPixmap(QString(":/images/phone-trans.png"), c).getPixmap());
            m_lblphones[phoneid]->setToolTip(tr("Phone %1 : %2")
                                             .arg(pi->number())
                                             .arg(longname));
        }
    }
}

void PeerWidget::setAgentToolTip(const QString &agentnum, const QStringList &queues)
{
    if ((m_agentlbl) && (agentnum != "")) {
        m_agentlbl->setToolTip(tr("Agent %1\n%n Queue(s) %2", "",
                               queues.size()).arg(agentnum).arg(queues.join(",")));
    }
}

/*! \brief change displayed name
 */
void PeerWidget::setName(const QString &/*name*/)
{
    QString text = tr("(No callerid yet)");

    if (!m_ui->fullname().isEmpty() && !m_ui->phoneNumber().isEmpty()) {
        text = tr("%1 <%2>").arg(m_ui->fullname()).arg(m_ui->phoneNumber());
    } else if (!m_ui->fullname().isEmpty()) {
        text = m_ui->fullname();
    } else if (!m_ui->phoneNumber().isEmpty()) {
        text = m_ui->phoneNumber();
    }

    QFontMetrics fm(m_textlbl->font());
    if (m_textlbl->width() < fm.width(text)) {
        m_textlbl->setToolTip(text);
        m_textlbl->setText(m_textlbl->fontMetrics().elidedText(text, Qt::ElideRight, m_textlbl->width()));
    } else {
        m_textlbl->setText(text);
    }
}

bool PeerWidget::pOverMobileLbl(const QPoint &p)
{
    if (m_mobilelbl) {
        return m_mobilelbl->rect().translated(m_mobilelbl->pos()).contains(p);
    }
    return false;
}

ChitchatButton::ChitchatButton(QWidget *parent, UserInfo **peerUi)
    : QPushButton(parent), m_ui(peerUi)
{
}

bool ChitchatButton::event(QEvent *e)
{
    /* Don't allow an user to open a chitchat windows
     * with someone non connected or with himself */
    if ((e->type() == QEvent::MouseButtonPress) ||
        (e->type() == QEvent::MouseButtonDblClick)) {
        if (((*m_ui)->availstate() == "xivo_unknown") ||
            (b_engine->getFullId() == (*m_ui)->userid())) {
            return true;
        }
    }

    return QPushButton::event(e);
}
