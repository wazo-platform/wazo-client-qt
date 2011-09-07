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
    : XLet(parent)
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
    m_agent->hide();

    m_voicemail = new IdentityVoiceMail(this);
    m_voicemail->hide();

    m_glayout->setSpacing(0);
    m_glayout->setMargin(0);

    m_col_user = 0;
    m_col_vm = m_col_user + 2;
    m_col_agent = m_col_vm + 1;
    m_col_phone = m_col_agent + 1;

    m_iconAlign = Qt::AlignHCenter | Qt::AlignTop;
    m_textAlignVCenter = Qt::AlignLeft | Qt::AlignVCenter;

    setupIcons();
    m_glayout->addWidget(m_agent, 0, m_col_agent, 3, 1);
    m_glayout->addWidget(m_voicemail, 0, m_col_vm, 3, 1);

    m_functions = b_engine->getConfig().keys();
    setGuiOptions(b_engine->getGuiOptions("merged_gui"));

    connect(b_engine, SIGNAL(forwardUpdated(const QString &, const QVariant &)),
            this, SLOT(setForward(const QString &, const QVariant &)));
    connect(b_engine, SIGNAL(updateUserConfig(const QString &)),
            this, SLOT(updateUserConfig(const QString &)));
    connect(b_engine, SIGNAL(updateUserStatus(const QString &)),
            this, SLOT(updateUserStatus(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
            this, SLOT(updatePhoneConfig(const QString &)));
    connect(b_engine, SIGNAL(updateAgentConfig(const QString &)),
            m_agent, SLOT(updateAgentConfig(const QString &)));
    connect(b_engine, SIGNAL(updateAgentStatus(const QString &)),
            m_agent, SLOT(updateAgentStatus(const QString &)));
    connect(b_engine, SIGNAL(updateVoiceMailConfig(const QString &)),
            m_voicemail, SLOT(updateVoiceMailConfig(const QString &)));
    connect(b_engine, SIGNAL(updateVoiceMailStatus(const QString &)),
            m_voicemail, SLOT(updateVoiceMailStatus(const QString &)));
    connect(b_engine, SIGNAL(localUserInfoDefined()), this, SLOT(updatePresence()));
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
    m_glayout->setColumnStretch(10, 1);
}

void IdentityDisplay::setGuiOptions(const QVariantMap & optionsMap)
{
    if (optionsMap.contains("xlet.identity.fontname") && optionsMap.contains("xlet.identity.fontsize"))
        m_gui_font = QFont(optionsMap.value("xlet.identity.fontname").toString(),
                           optionsMap.value("xlet.identity.fontsize").toInt());
    if (optionsMap.contains("xlet.identity.iconsize"))
        m_gui_buttonsize = optionsMap.value("xlet.identity.iconsize").toInt();

    m_agent->setAllowedActions(b_engine->getConfig("xlet.identity.logagent").toBool(),
                               b_engine->getConfig("xlet.identity.pauseagent").toBool());

    setFont(m_gui_font);

    m_loginkind = optionsMap.value("loginkind").toUInt();
}

void IdentityDisplay::updatePresence()
{
    if (! m_ui) return;
    QString presence = m_ui->availstate();
    QVariantMap presencemap = b_engine->getOptionsUserStatus();

    m_presencevalue->hide();
    if (! m_functions.contains("checked_function.presence"))
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
void IdentityDisplay::setOpt()
{
    if (m_ui) {
        m_svcstatus["enablednd"] = m_ui->enablednd();
        m_svcstatus["incallfilter"] = m_ui->incallfilter();
        m_svcstatus["callrecord"] = m_ui->callrecord();
        m_svcstatus["enablevoicemail"] = m_ui->enablevoicemail();
        m_svcstatus["unc-enabled"] = m_ui->enableunc();
        m_svcstatus["unc-number"] = m_ui->destunc();
        m_svcstatus["rna-enabled"] = m_ui->enablerna();
        m_svcstatus["rna-number"] = m_ui->destrna();
        m_svcstatus["busy-enabled"] = m_ui->enablebusy();
        m_svcstatus["busy-number"] = m_ui->destbusy();
        svcSummary();
    }
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
    foreach (const QString & key, m_identityphones.keys()) {
        m_identityphones[key]->svcSummary(m_svcstatus);
    }
}

/*! \brief update phone config
 */
void IdentityDisplay::updatePhoneConfig(const QString & xphoneid)
{
    if (m_ui == NULL)
        return;
    const PhoneInfo * phoneinfo = b_engine->phone(xphoneid);
    if (phoneinfo == NULL)
        return;
    QString iduserfeatures = phoneinfo->iduserfeatures();
    if (iduserfeatures != m_ui->id())
        return;
    m_phonenum->setText(b_engine->phonenumbers(m_ui).join(", "));
}

/*! \brief update user config
 */
void IdentityDisplay::updateUserConfig(const QString & xuserid)
{
    if (xuserid != m_xuserid)
        return;
    if (m_ui == NULL)
        return;
    m_user->setText(QString("%1 (%2)").arg(m_ui->fullname()).arg(m_ui->ipbxid()));

    foreach (QString xphoneid, m_ui->phonelist()) {
        const PhoneInfo * phoneinfo = b_engine->phone(xphoneid);
        if (phoneinfo == NULL)
            continue;
        if (! m_identityphones.contains(xphoneid)) {
            m_identityphones[xphoneid] = new IdentityPhone(this);
            connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
                    m_identityphones[xphoneid], SLOT(updatePhoneConfig(const QString &)));
            connect(b_engine, SIGNAL(updatePhoneStatus(const QString &)),
                    m_identityphones[xphoneid], SLOT(updatePhoneStatus(const QString &)));
            connect(b_engine, SIGNAL(updateChannelStatus(const QString &)),
                    m_identityphones[xphoneid], SLOT(updateChannelStatus(const QString &)));
            m_glayout->addWidget(m_identityphones[xphoneid], 0, m_col_phone ++, 3, 1);
        }
        m_identityphones[xphoneid]->setPhoneId(xphoneid);
    }
    m_phonenum->setText(b_engine->phonenumbers(m_ui).join(", "));
    m_phonenum->setToolTip(tr("Server: %1\n"
                              "Context: %2")
                           .arg(m_ui->ipbxid())
                           .arg(m_ui->context()));

    if (m_ui->voicemailid().isEmpty())
        m_voicemail->hide();
    else {
        m_voicemail->show();
        m_voicemail->svcSummary(m_svcstatus, m_ui);
        m_voicemail->setVoiceMailId(m_ui->xvoicemailid());
    }

    // changes the "watched agent" only if no one else has done it before
    b_engine->changeWatchedAgent(m_ui->xagentid(), false);

    foreach(QString xphoneid, m_ui->phonelist()) {
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

    if (! m_ui->agentid().isEmpty())
        m_agent->show();
    m_agent->setAgentId(m_ui->xagentid());
}

/*!
 * Update the availability dropdown list when our status is updated
 * \param xuserid The updated user's XiVO id
 */
void IdentityDisplay::updateUserStatus(const QString & xuserid)
{
    if (m_ui && m_ui->xid() == xuserid) {
        updatePresence();
        setOpt();
    }
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
