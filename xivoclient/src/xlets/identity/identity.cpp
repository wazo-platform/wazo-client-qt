/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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

#include <QAction>
#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QSizePolicy>

#include <baseengine.h>
#include <message_factory.h>
#include <storage/userinfo.h>
#include <storage/phoneinfo.h>
#include <storage/agentinfo.h>
#include <storage/channelinfo.h>
#include <storage/queueinfo.h>

#include "identity.h"
#include "identityphone.h"
#include "identityvoicemail.h"

QIcon IdentityDisplay::m_show_icon(":/images/show.svg");
QIcon IdentityDisplay::m_hide_icon(":/images/hide.svg");

const QString icon_user_unlogged = "xivo-black";
const QString icon_user_logged = "xivo-transp";
const QString icon_agent_logged = "xivo-green";
const QString icon_agent_paused = "xivo-red";

XLet* XLetIdentityPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/identity_%1");
    return new IdentityDisplay(parent);
}

IdentityDisplay::IdentityDisplay(QWidget *parent)
    : XLet(parent, tr("Identity")),
      m_agent_menu(new QMenu(this)),
      m_presence_mapper(new QSignalMapper(this)),
      m_presence_menu(new QMenu(this))
{
    setAccessibleName( tr("Current User Panel") );
    setObjectName("identityXlet");

    this->ui.setupUi(this);

    this->ui.presence_button->setMenu(m_presence_menu);
    connect(this->ui.fold_button, SIGNAL(toggled(bool)),
            this, SLOT(foldToggle(bool)));

    bool may_log_agent = b_engine->getConfig("xlet.identity.logagent").toBool();
    bool may_pause_agent = b_engine->getConfig("xlet.identity.pauseagent").toBool();

    this->ui.agent_button->setMenu(m_agent_menu);
    QAction *action = NULL;
    if (may_log_agent) {
        action = this->m_agent_menu->addAction(tr("Login"));
        connect(action, SIGNAL(triggered()),
                this, SLOT(login()));
        action = this->m_agent_menu->addAction(tr("Logout"));
        connect(action, SIGNAL(triggered()),
                this, SLOT(logout()));
    }
    if (may_pause_agent) {
        action = this->m_agent_menu->addAction(tr("Pause"));
        connect(action, SIGNAL(triggered()),
                this, SLOT(pause()));
        action = this->m_agent_menu->addAction(tr("Unpause"));
        connect(action, SIGNAL(triggered()),
                this, SLOT(unpause()));
    }

    /*m_gui_buttonsize = 16;

    QVBoxLayout* vboxLayout = new QVBoxLayout(this);
    QFrame* m_identitybck = new QFrame(this);
    m_identitybck->setObjectName("identitybck");
    vboxLayout->addWidget(m_identitybck);

    m_glayout = new QGridLayout(m_identitybck);
    m_glayout->setObjectName("identitylayout");

    m_user = new QLabel(this);
    m_user->setObjectName("fullname");
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    m_phonenum = new QLabel(this);
    m_phonenum->setObjectName("phonenum");

    int last_column = 0;
    m_fold_button.setFlat(true);
    m_fold_button.setIcon(QIcon(":/images/hide.svg"));
    m_fold_button.setCheckable(true);
    connect(&m_fold_button, SIGNAL(toggled(bool)),
            this, SLOT(foldToggle(bool)));

    m_icon_user = new QLabel(this);
    m_icon_user->setPixmap(QPixmap(":/images/identity/identity-user.png"));
    m_icon_user->setContentsMargins(0, 0, 5, 0);

    m_voicemail = new IdentityVoiceMail(this);
    m_voicemail->hide();

    m_glayout->addWidget(&m_fold_button, 0, last_column, 3, 1);
    last_column ++;
    m_iconAlign = Qt::AlignHCenter | Qt::AlignTop;
    m_textAlignVCenter = Qt::AlignLeft | Qt::AlignVCenter;

    m_glayout->addWidget(m_icon_user, 0, last_column, 3, 1, m_iconAlign);
    last_column ++;

    int idline = 0;
    m_glayout->addWidget(m_user, idline, last_column, m_textAlignVCenter);
    idline ++;
    m_glayout->addWidget(m_phonenum, idline, last_column, m_textAlignVCenter);
    idline ++;


    m_glayout->addWidget(m_voicemail, 0, last_column, 3, 1);
    last_column ++;

    m_col_phone = last_column;

    m_glayout->setColumnStretch(0, 0);
    m_glayout->setColumnStretch(1, 0);
    m_glayout->setColumnStretch(10, 1);
    m_glayout->setSpacing(0);
    m_glayout->setMargin(0);

    setGuiOptions();

    connect(b_engine, SIGNAL(updateUserConfig(const QString &)),
            this, SLOT(updateUserConfig(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
            this, SLOT(updatePhoneConfig(const QString &)));
    connect(b_engine, SIGNAL(updateVoiceMailConfig(const QString &)),
            m_voicemail, SLOT(updateVoiceMailConfig(const QString &)));
    connect(b_engine, SIGNAL(updateVoiceMailStatus(const QString &)),
            m_voicemail, SLOT(updateVoiceMailStatus(const QString &)));*/

    connect(b_engine, SIGNAL(updateAgentStatus(const QString &)),
            this, SLOT(updateAgentStatus(const QString &)));
    connect(b_engine, SIGNAL(settingsChanged()),
            this, SLOT(updatePresenceVisibility()));
    connect(b_engine, SIGNAL(updateUserStatus(const QString &)),
            this, SLOT(updateUserStatus(const QString &)));
    connect(b_engine, SIGNAL(localUserInfoDefined()), this, SLOT(updatePresenceList()));
    connect(m_presence_mapper, SIGNAL(mapped(const QString &)),
            this, SLOT(setPresence(const QString &)));
    connect(b_engine, SIGNAL(updateUserConfig(const QString &)),
            this, SLOT(updateAgentVisibility()));
}

void IdentityDisplay::foldToggle(bool fold)
{
    if (fold) {
        emit showOnlyMeRequested();
        this->ui.fold_button->setIcon(m_show_icon);
    } else {
        emit showOthersRequested();
        this->ui.fold_button->setIcon(m_hide_icon);
    }
}

void IdentityDisplay::setGuiOptions()
{
    // These options can be set in the web-i in cti server > profile > preference
    if (b_engine->getConfig().contains("xlet.identity.fontname") && b_engine->getConfig().contains("xlet.identity.fontsize"))
        m_gui_font = QFont(b_engine->getConfig("xlet.identity.fontname").toString(),
                           b_engine->getConfig("xlet.identity.fontsize").toInt());
    if (b_engine->getConfig().contains("xlet.identity.iconsize"))
        m_gui_buttonsize = b_engine->getConfig("xlet.identity.iconsize").toInt();

    setFont(m_gui_font);
}

void IdentityDisplay::updatePresenceList()
{
    this->ui.presence_button->menu()->clear();

    if (! m_ui) {
        return;
    }
    QString presence = m_ui->availstate();
    QVariantMap presencemap = b_engine->getOptionsUserStatus();

    if (! presencemap.contains(presence)) {
        return;
    }

    QStringList allowed_presences = presencemap.value(presence).toMap().value("allowed").toStringList();
    // in case there's no allowed state, we get a non empty list with one empty string ("")
    allowed_presences.removeAll("");
    // A state should not have to be authorised to change to itself, so we add it by default
    if (! allowed_presences.contains(presence)) {
        allowed_presences << presence;
    }
    foreach (QString presence_name, allowed_presences) {
        QString presence_display_name = presencemap.value(presence_name).toMap().value("longname").toString();

        QAction *action = this->ui.presence_button->menu()->addAction(presence_display_name);
        m_presence_mapper->setMapping(action, presence_name);
        connect(action, SIGNAL(triggered()),
                m_presence_mapper, SLOT(map()));
    }
}

void IdentityDisplay::updatePresenceVisibility()
{
    bool presenceEnabled = b_engine->getConfig("checked_function.presence").toBool();
    this->ui.presence_button->setVisible(presenceEnabled);
}

void IdentityDisplay::updateAgentVisibility()
{
    bool client_logged_with_agent = (b_engine->getConfig("guioptions.loginkind").toUInt() != 0);
    bool user_has_agent = (! m_ui->agentid().isEmpty());
    if (client_logged_with_agent and user_has_agent) {
        this->ui.agent_button->show();
    } else {
        this->ui.agent_button->hide();
    }
}

/*! \brief updates the boolean services
 */
void IdentityDisplay::setOpt()
{
    if (m_ui) {
        m_svcstatus["enablednd"] = m_ui->enablednd();
        m_svcstatus["incallfilter"] = m_ui->incallfilter();
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
    m_user->setText(QString("%1").arg(m_ui->fullname()));
    m_user->setToolTip(tr("IPBXid: %1").arg(m_ui->ipbxid()));
    // to add some day in the tooltip : entity name

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
            m_glayout->addWidget(m_identityphones[xphoneid], 0, m_col_phone ++, 3, 1);
        }
        m_identityphones[xphoneid]->setPhoneId(xphoneid);
    }
    m_phonenum->setText(b_engine->phonenumbers(m_ui).join(", "));
    m_phonenum->setToolTip(tr("IPBXid: %1")
                           .arg(m_ui->ipbxid()));

    if (m_ui->voicemailid().isEmpty())
        m_voicemail->hide();
    else {
        m_voicemail->show();
        m_voicemail->svcSummary(m_svcstatus, m_ui);
        m_voicemail->setVoiceMailId(m_ui->xvoicemailid());
    }

    // changes the "watched agent" only if no one else has done it before
    b_engine->changeWatchedAgent(m_ui->xagentid(), false);
}

void IdentityDisplay::updateUserStatus(const QString & /*xuserid*/)
{
    updatePresenceList();
    setOpt();
}

void IdentityDisplay::updateAgentStatus(const QString & agent_id)
{
    if (! m_ui) {
        return;
    }
    if(agent_id != m_ui->xagentid()) {
        return;
    }
    const AgentInfo * agentinfo = b_engine->agent(agent_id);
    if (agentinfo == NULL)
        return;

    if (agentinfo->logged()) {
            emit setAppIcon(icon_agent_logged);
    } else {
            emit setAppIcon(icon_user_logged);
    }

    QString image_path;

    if (agentinfo->logged()) {
        image_path = ":/images/agent-on.svg";
    } else {
        image_path = ":/images/agent-off.svg";
    }

    this->ui.agent_button->setIcon(QIcon(image_path));
}

void IdentityDisplay::setPresence(const QString &new_presence)
{
    b_engine->setAvailState(new_presence, false);
}

void IdentityDisplay::doGUIConnects(QWidget * mainwindow)
{
    connect( this, SIGNAL(setAppIcon(const QString &)),
             mainwindow, SLOT(setAppIcon(const QString &)) );
}

void IdentityDisplay::login()
{
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = "agentlogin";
    ipbxcommand["agentphonenumber"] = b_engine->getConfig("agentphonenumber");
    b_engine->ipbxCommand(ipbxcommand);
}

void IdentityDisplay::logout()
{
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = "agentlogout";
    b_engine->ipbxCommand(ipbxcommand);
}

void IdentityDisplay::pause()
{
    if (! m_ui) {
        return;
    }
    const QString &agent_id = m_ui->xagentid();
    const AgentInfo *agent = b_engine->agent(agent_id);
    if (! agent) {
        return;
    }
    QVariantMap message = MessageFactory::pauseAgentInAllQueues(agent_id, agent->ipbxid());
    b_engine->sendJsonCommand(message);
}

void IdentityDisplay::unpause()
{
    if (! m_ui) {
        return;
    }
    const QString &agent_id = m_ui->xagentid();
    const AgentInfo *agent = b_engine->agent(m_ui->xagentid());
    if (! agent) {
        return;
    }
    QVariantMap message = MessageFactory::unpauseAgentInAllQueues(agent_id, agent->ipbxid());
    b_engine->sendJsonCommand(message);
}
