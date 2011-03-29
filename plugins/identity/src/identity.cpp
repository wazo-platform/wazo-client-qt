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

#include <QAction>
#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QRegExp>

#include "baseengine.h"
#include "identity.h"
#include "userinfo.h"
#include "phoneinfo.h"
#include "agentinfo.h"
#include "queueinfo.h"
#include "identityagent.h"
#include "identityphone.h"
#include "identityvoicemail.h"

Q_EXPORT_PLUGIN2(xletidentityplugin, XLetIdentityPlugin);

XLet* XLetIdentityPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/identity_%1");
    return new IdentityDisplay(parent);
}

IdentityDisplay::IdentityDisplay(QWidget *parent)
    : XLet(parent), m_ui(NULL)
{
    setTitle( tr("Identity") );
    setAccessibleName( tr("Current User Panel") );
    m_gui_buttonsize = 16;

    m_glayout = new QGridLayout(this);
    // m_glayout->setMargin(0);
    m_user = new QLabel(this);
    m_user->setObjectName("fullname");
    //m_user->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    m_phonenum = new QLabel(this);
    m_presencevalue = new QComboBox(this);
    m_presencevalue->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_presencevalue->setProperty("function", "presence");
    m_presencevalue->setContentsMargins(0, 0, 10, 0);

    connect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(idxChanged(const QString &)));

    m_icon_user = new QLabel(this);

    m_icon_user->setPixmap(QPixmap(":/in/identity-user.png"));

    m_icon_user->setContentsMargins(0, 0, 5, 0);

    m_agent = new IdentityAgent(this);
    connect(m_agent, SIGNAL(setSystrayIcon(const QString &)),
            this, SIGNAL(setSystrayIcon(const QString &)));
    m_agent->setContentsMargins(5, 0, 5, 0);

    m_phone = new IdentityPhone(this);

    m_voicemail = new IdentityVoiceMail(this);
    m_voicemail->hide();

    m_glayout->setSpacing(0);
    m_glayout->setMargin(0);

    m_col_user = 0;
    m_col_agent = 2;
    m_col_phone = 3;
    m_col_vm = 4;
    m_col_last = 5;

    m_iconAlign = Qt::AlignHCenter | Qt::AlignTop;
    m_textAlignVCenter = Qt::AlignLeft | Qt::AlignVCenter;

    setupIcons();
    m_glayout->addWidget(m_agent, 0, m_col_agent, 3, 1);
    m_glayout->addWidget(m_phone, 0, m_col_phone, 3, 1);
    m_glayout->addWidget(m_voicemail, 0, m_col_vm, 3, 1);

    // m_agent->hide();

    m_functions = b_engine->getGuiOptions("server_funcs").value("functions").toStringList();
    setGuiOptions(b_engine->getGuiOptions("merged_gui"));

    // connect signals/slots
    connect(b_engine, SIGNAL(updatePresence()),
            this, SLOT(updatePresence()));

    connect(b_engine, SIGNAL(optChanged(const QString &, bool)),
            this, SLOT(setOpt(const QString &, bool)));
    connect(b_engine, SIGNAL(forwardUpdated(const QString &, const QVariant &)),
            this, SLOT(setForward(const QString &, const QVariant &)));

    connect(b_engine, SIGNAL(updateUserConfig(const QString &)),
            this, SLOT(updateUserConfig(const QString &)));
    connect(b_engine, SIGNAL(updateUserStatus(const QString &)),
            this, SLOT(updateUserStatus(const QString &)));

    connect(b_engine, SIGNAL(updateAgentConfig(const QString &)),
            m_agent, SLOT(updateAgentConfig(const QString &)));
    connect(b_engine, SIGNAL(updateAgentStatus(const QString &)),
            m_agent, SLOT(updateAgentStatus(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
            m_phone, SLOT(updatePhoneConfig(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneStatus(const QString &)),
            m_phone, SLOT(updatePhoneStatus(const QString &)));
    connect(b_engine, SIGNAL(updateChannelStatus(const QString &)),
            m_phone, SLOT(updateChannelStatus(const QString &)));

    b_engine->setAvailState("available", true);
    updatePresence();
}

void IdentityDisplay::setupIcons()
{
    m_glayout->addWidget(m_icon_user, 0, m_col_user, 3, 1, m_iconAlign);
    int idline = 0;
    m_glayout->addWidget(m_user, idline, m_col_user + 1, m_textAlignVCenter);
    idline ++;
    m_glayout->addWidget(m_phonenum, idline, m_col_user + 1, m_textAlignVCenter);
    idline ++;
    m_glayout->addWidget(m_presencevalue, idline, m_col_user + 1, m_textAlignVCenter);

    m_glayout->setColumnStretch(0, 0);
    m_glayout->setColumnStretch(1, 0);
    m_glayout->setColumnStretch(4, 1);
}

void IdentityDisplay::setGuiOptions(const QVariantMap & optionsMap)
{
    if (optionsMap.contains("xlet.identity.fontname") && optionsMap.contains("xlet.identity.fontsize"))
        m_gui_font = QFont(optionsMap.value("xlet.identity.fontname").toString(),
                           optionsMap.value("xlet.identity.fontsize").toInt());
    if (optionsMap.contains("xlet.identity.iconsize"))
        m_gui_buttonsize = optionsMap.value("xlet.identity.iconsize").toInt();

    m_agent->setAllowedActions(optionsMap.value("xlet.identity.logagent").toBool(),
                               optionsMap.value("xlet.identity.pauseagent").toBool());

    setFont(m_gui_font);

    m_loginkind = optionsMap.value("loginkind").toUInt();
}

void IdentityDisplay::updatePresence()
{
    QString presence = b_engine->getAvailState();
    QVariantMap presencemap = b_engine->getOptionsUserStatus();
    qDebug() << Q_FUNC_INFO;

    m_presencevalue->hide();
    if (! m_functions.contains("presence"))
        return;

    disconnect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
               this, SLOT(idxChanged(const QString &)));

    m_presencevalue->clear();
    m_presence_names.clear();

    if (presencemap.contains(presence)) {
        QVariantMap details = presencemap.value(presence).toMap();
        QStringList allowedlist = details.value("allowed").toStringList();
        int idx = 0;
        foreach (QString presencestate, allowedlist) {
            QVariantMap pdetails = presencemap.value(presencestate).toMap();
            QString longname = pdetails.value("longname").toString();
            m_presencevalue->addItem(longname);
            m_presence_names[presencestate] = longname;
            if (presence == presencestate)
                m_presencevalue->setCurrentIndex(idx);
            idx ++;
        }
    }

    connect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(idxChanged(const QString &)));
    m_presencevalue->show();
}

// XXX disable agent update on identityagent when loginkind = 0 ? what if logged in nevertheless ?
// XXX show() agent


/*! \brief updates the boolean services
 */
void IdentityDisplay::setOpt(const QString & capa, bool b)
{
    if ((capa == "enablednd") || (capa == "incallfilter") || (capa == "callrecord") || (capa == "enablevm"))
        m_svcstatus[capa] = b;
    svcSummary();
}

/*! \brief updates the boolean+value services
 */
void IdentityDisplay::setForward(const QString & capa, const QVariant & value)
{
    if ((capa == "unc") || (capa == "busy") || (capa == "rna")) {
        m_svcstatus[capa + "-enabled"] = value.toMap().value("enabled");
        m_svcstatus[capa + "-number"] = value.toMap().value("number");
    }
    svcSummary();
}

/*! \brief updates the display of Services data
 */
void IdentityDisplay::svcSummary()
{
    if (m_phone)
        m_phone->svcSummary(m_svcstatus);
    if (m_ui) {
        QStringList vm = m_ui->mwi();
        if (vm.size() > 2) {
            m_voicemail->svcSummary(m_svcstatus, m_ui);
        }
    }
    return;
}

/*! \brief update user status
 */
void IdentityDisplay::updateUserConfig(const QString & xuserid)
{
    m_ui = b_engine->getXivoClientUser();
    m_xuserid = m_ui->xid();
    if (xuserid != m_xuserid)
        return;
    m_user->setText(m_ui->fullname());
    m_phonenum->setText(m_ui->phoneNumber());
    m_phonenum->setToolTip(tr("Server: %1\n"
                              "Context: %2")
                           .arg(m_ui->ipbxid())
                           .arg(m_ui->context()));
    m_phone->setPhoneId(m_ui->phonelist().join(""));
    QStringList vm = m_ui->mwi();
    if (vm.size() > 2) {
        m_voicemail->show();
        m_voicemail->svcSummary(m_svcstatus, m_ui);
        m_voicemail->setOldNew(vm[1], vm[2]);
    }
    // changes the "watched agent" only if no one else has done it before
    b_engine->changeWatchedAgent(m_ui->xagentid(), false);


    QString ipbxid = m_ui->ipbxid();
    foreach(QString phoneid, m_ui->phonelist()) {
        QString xphoneid = QString("%1/%2").arg(ipbxid).arg(phoneid);
        const PhoneInfo * phoneinfo = b_engine->phone(xphoneid);
        if (phoneinfo == NULL)
            continue;
        foreach (const QString channel, phoneinfo->channels()) {
            const ChannelInfo * channelinfo = b_engine->channels().value(channel);
            if (channelinfo == NULL)
                continue;
            QString status = channelinfo->commstatus();
            QString todisplay = channelinfo->peerdisplay();
            // QString direction = channelinfo->direction();
            // int timestamp = channelinfo->timestamp();

            QPixmap square_comm(25, 3);
            square_comm.fill(channelinfo->isholded() ? Qt::darkGreen : Qt::green);
            if (status == "hangup") {
                todisplay = tr("(Line %1)").arg(channelinfo->linenumber());
                square_comm.fill(Qt::black);
            }
        }
    }

    m_agent->setAgentId(m_ui->xagentid());
}

void IdentityDisplay::updateUserStatus(const QString & xuserid)
{
    // qDebug() << Q_FUNC_INFO << xuserid;
    const UserInfo * userinfo = b_engine->user(xuserid);
    if (userinfo == NULL)
        return;
}

void IdentityDisplay::idxChanged(const QString & newidx)
{
    QString function = sender()->property("function").toString();
    qDebug() << Q_FUNC_INFO << newidx << sender() << function << m_presence_names;
    if (function == "presence") {
        foreach (QString avstate, m_presence_names.keys())
            if (m_presence_names[avstate] == newidx)
                b_engine->setAvailState(avstate, false);
    }
}

void IdentityDisplay::doGUIConnects(QWidget * mainwindow)
{
    connect( this, SIGNAL(setSystrayIcon(const QString &)),
             mainwindow, SLOT(setSystrayIcon(const QString &)) );
}
