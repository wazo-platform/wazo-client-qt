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

#include <baseengine.h>
#include <message_factory.h>

#include <storage/agentinfo.h>
#include <storage/phoneinfo.h>
#include <storage/voicemailinfo.h>

#include "identity.h"

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
      m_hide_icon(":/identity/images/hide.svg"),
      m_show_icon(":/identity/images/show.svg"),
      m_presence_mapper(new QSignalMapper(this)),
      m_agent_menu(new QMenu(this)),
      m_presence_menu(new QMenu(this))
{
    setAccessibleName(tr("Current User Panel"));

    this->ui.setupUi(this);

    this->ui.presence_button->setMenu(m_presence_menu);
    this->m_presence_menu->setAttribute(Qt::WA_TranslucentBackground);
    connect(m_presence_mapper, SIGNAL(mapped(const QString &)),
            this, SLOT(setPresence(const QString &)));

    this->ui.agent_button->setMenu(m_agent_menu);
    this->fillAgentMenu(m_agent_menu);
    this->m_agent_menu->setAttribute(Qt::WA_TranslucentBackground);

    connect(b_engine, SIGNAL(updateUserConfig(const QString &)),
            this, SLOT(updateUserConfig(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
            this, SLOT(updatePhoneConfig(const QString &)));
    connect(b_engine, SIGNAL(updateVoiceMailConfig(const QString &)),
            this, SLOT(updateVoiceMailConfig(const QString &)));

    connect(b_engine, SIGNAL(updateAgentStatus(const QString &)),
            this, SLOT(updateAgentStatus(const QString &)));
    connect(b_engine, SIGNAL(updateUserStatus(const QString &)),
            this, SLOT(updateUserStatus(const QString &)));
    connect(b_engine, SIGNAL(updateVoiceMailStatus(const QString &)),
            this, SLOT(updateVoiceMailStatus(const QString &)));

    connect(b_engine, SIGNAL(settingsChanged()),
            this, SLOT(updatePresenceVisibility()));
}

void IdentityDisplay::fillAgentMenu(QMenu *menu)
{
    bool may_log_agent = b_engine->getConfig("xlet.identity.logagent").toBool();
    bool may_pause_agent = b_engine->getConfig("xlet.identity.pauseagent").toBool();

    QAction *action = NULL;
    if (may_log_agent) {
        action = menu->addAction(tr("Login"));
        connect(action, SIGNAL(triggered()),
                this, SLOT(login()));
        menu->addSeparator();
        action = menu->addAction(tr("Logout"));
        connect(action, SIGNAL(triggered()),
                this, SLOT(logout()));
    }
    if (may_log_agent && may_pause_agent) {
        menu->addSeparator();
    }
    if (may_pause_agent) {
        action = menu->addAction(tr("Pause"));
        connect(action, SIGNAL(triggered()),
                this, SLOT(pause()));
        menu->addSeparator();
        action = menu->addAction(tr("Unpause"));
        connect(action, SIGNAL(triggered()),
                this, SLOT(unpause()));
    }
}

void IdentityDisplay::doGUIConnects(QWidget * mainwindow)
{
    connect(this, SIGNAL(setAppIcon(const QString &)),
            mainwindow, SLOT(setAppIcon(const QString &)));
}

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
    this->updateNameTooltip();
}

void IdentityDisplay::updateUserConfig(const QString & xuserid)
{
    if (xuserid != m_xuserid)
        return;
    if (m_ui == NULL)
        return;
    this->ui.name->setText(m_ui->fullname());
    this->updateNameTooltip();
    this->updateAgentVisibility();
    this->updateVoiceMailVisibility();

    b_engine->changeWatchedAgent(m_ui->xagentid(), false);
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

void IdentityDisplay::updateVoiceMailVisibility()
{
    if (! m_ui) {
        return;
    }
    bool has_voicemail = (! m_ui->voicemailid().isEmpty());
    this->ui.voicemail_button->setVisible(has_voicemail);
    this->ui.voicemail_label->setVisible(has_voicemail);
    this->ui.voicemail_number->setVisible(has_voicemail);
    this->ui.voicemail_messages->setVisible(has_voicemail);
}

void IdentityDisplay::updateVoiceMailConfig(const QString & xvoicemailid)
{
    if (! m_ui) {
        return;
    }
    if (xvoicemailid != m_ui->xvoicemailid()) {
        return;
    }
    const VoiceMailInfo * voicemailinfo = b_engine->voicemail(xvoicemailid);
    if (voicemailinfo == NULL) {
        return;
    }
    this->requestVoicemailMessageCount(voicemailinfo);
    this->ui.voicemail_number->setText(voicemailinfo->mailbox());
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

    if (agentinfo->logged()) {
        this->ui.agent_button->setIcon(QIcon(":/images/agent-on.svg"));
    } else {
        this->ui.agent_button->setIcon(QIcon(":/images/agent-off.svg"));
    }
}

void IdentityDisplay::updateUserStatus(const QString & xuserid)
{
    if (xuserid != m_xuserid) {
        return;
    }
    this->updateOptions();
    this->updateCurrentPresence();
    this->updatePresenceList();
}

void IdentityDisplay::updateVoiceMailStatus(const QString & xvoicemailid)
{
    if (! m_ui) {
        return;
    }
    if (xvoicemailid != m_ui->xvoicemailid()) {
        return;
    }
    const VoiceMailInfo * voicemail = b_engine->voicemail(xvoicemailid);
    if (voicemail == NULL) {
        return;
    }
    this->ui.voicemail_messages->setText(tr("%n message(s)", "unread voicemail messages", voicemail->newMessages()));
}

void IdentityDisplay::updateCurrentPresence() {
    if (! m_ui) {
        return;
    }

    QString presence = m_ui->availstate();
    QVariantMap presencemap = b_engine->getOptionsUserStatus();
    QString presence_color_string = presencemap.value(presence).toMap().value("color").toString();
    QColor presence_color = QColor(presence_color_string);

    QPixmap presence_image = this->presenceIcon(presence_color);

    this->ui.presence_button->setIcon(presence_image);
}

QPixmap IdentityDisplay::presenceIcon(const QColor & presence_color) {
    QPixmap presence_arrow = QIcon(":/identity/images/down-arrow.svg").pixmap(QSize(10, 5));
    QPainter tint_painter(&presence_arrow);
    tint_painter.setRenderHint(QPainter::Antialiasing);
    tint_painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    tint_painter.fillRect(presence_arrow.rect(), "#2c2927");
    tint_painter.end();

    QPixmap presence_image = QPixmap(this->ui.presence_button->size());
    presence_image.fill(Qt::transparent);

    QRect arrow_rect = presence_arrow.rect();
    arrow_rect.moveCenter(presence_image.rect().center());

    QPainter presence_painter(&presence_image);
    presence_painter.setBrush(presence_color);
    presence_painter.setPen(presence_color);
    presence_painter.setRenderHint(QPainter::Antialiasing);
    presence_painter.drawEllipse(presence_image.rect().adjusted(1, 1, -2, -2).adjusted(3, 3, -3, -3));
    presence_painter.drawPixmap(arrow_rect, presence_arrow);
    presence_painter.end();

    return presence_image;
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
    if (allowed_presences.isEmpty()) {
        return;
    }

    QString first_presence_name = allowed_presences.takeFirst();
    this->addPresence(first_presence_name);

    foreach (QString presence_name, allowed_presences) {
        this->ui.presence_button->menu()->addSeparator();
        this->addPresence(presence_name);

    }
}

void IdentityDisplay::addPresence(const QString &presence_name)
{
    QVariantMap presencemap = b_engine->getOptionsUserStatus();
    QString presence_display_name = presencemap.value(presence_name).toMap().value("longname").toString();

    QAction *action = this->ui.presence_button->menu()->addAction(presence_display_name);
    m_presence_mapper->setMapping(action, presence_name);
    connect(action, SIGNAL(triggered()),
            m_presence_mapper, SLOT(map()));
}

void IdentityDisplay::updateOptions()
{
    if (! m_ui) {
        return;
    }
    if (m_ui->enablednd()) {
        this->ui.options->setText(tr("DND"));
        this->ui.options->setToolTip(tr("Do Not Disturb"));
    } else if (m_ui->enableunc()) {
        this->ui.options->setText(tr("UNC %1").arg(m_ui->destunc()));
        this->ui.options->setToolTip(tr("Unconditional Forward towards %1").arg(m_ui->destunc()));
    } else if (m_ui->enablebusy()) {
        this->ui.options->setText(tr("BUSY %1").arg(m_ui->destbusy()));
        this->ui.options->setToolTip(tr("Busy Forward towards %1").arg(m_ui->destbusy()));
    } else if (m_ui->enablerna()) {
        this->ui.options->setText(tr("FNA %1").arg(m_ui->destrna()));
        this->ui.options->setToolTip(tr("Non-Answer Forward towards %1").arg(m_ui->destrna()));
    } else if (m_ui->incallfilter()) {
        this->ui.options->setText(tr("CALL FILTER"));
        this->ui.options->setToolTip("");
    } else {
        this->ui.options->setText("AVAILABLE");
        this->ui.options->setToolTip("");
    }
}

void IdentityDisplay::updatePresenceVisibility()
{
    bool presenceEnabled = b_engine->getConfig("checked_function.presence").toBool();
    this->ui.presence_button->setVisible(presenceEnabled);
}

void IdentityDisplay::updateNameTooltip()
{
    QString phone_numbers = b_engine->phonenumbers(m_ui).join(", ");
    QString tooltip = QString("%1\n%2").arg(m_ui->fullname()).arg(phone_numbers);
    this->ui.name->setToolTip(tooltip);
}

void IdentityDisplay::setPresence(const QString &new_presence)
{
    b_engine->setAvailState(new_presence, false);
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

void IdentityDisplay::on_fold_button_toggled(bool fold)
{
    if (fold) {
        emit showOnlyMeRequested();
        this->ui.fold_button->setIcon(m_show_icon);
    } else {
        emit showOthersRequested();
        this->ui.fold_button->setIcon(m_hide_icon);
    }
}

void IdentityDisplay::on_voicemail_button_clicked()
{
    if (! m_ui) {
        return;
    }

    QString voicemail_id = m_ui->xvoicemailid();
    b_engine->actionDial(QString("vm_consult:%1").arg(voicemail_id));
}

void IdentityDisplay::on_call_input_returnPressed()
{
    QString extension = this->ui.call_input->text();
    if (extension.isEmpty())
        return;
    b_engine->actionDial(extension);
    this->ui.call_input->clear();
}


void IdentityDisplay::requestVoicemailMessageCount(const VoiceMailInfo *voicemail)
{
    if (voicemail == NULL) {
        return;
    }
    QVariantMap command;
    command["command"] = "mailboxcount";
    command["mailbox"] = voicemail->mailbox();
    command["context"] = voicemail->context();
    b_engine->ipbxCommand(command);
}
