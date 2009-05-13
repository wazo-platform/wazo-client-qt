/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QProgressBar>
#include <QPushButton>
#include <QRegExp>
#include <QScrollArea>

#include "baseengine.h"
#include "extendedlabel.h"
#include "identitydisplay.h"
#include "userinfo.h"
#include "phoneinfo.h"
#include "agentinfo.h"
#include "queueinfo.h"

QString icon_color_red = "xivo-red";
QString icon_color_black = "xivo-black";
QString icon_color_green = "xivo-green";

const QColor Orange = QColor(255, 128, 0);

/*! \brief Constructor
 */
IdentityDisplay::IdentityDisplay(BaseEngine * engine,
                                 const QVariant & options,
                                 QWidget * parent)
    : QWidget(parent), m_engine(engine),
      m_ui(NULL), m_nlines(0)
{
    setAccessibleName( tr("Current User Panel") );
    m_gui_buttonsize = 16;
    
    m_glayout = new QGridLayout(this);
    // m_glayout->setMargin(0);
    m_user = new QLabel(this);
    m_user->setObjectName("fullname");
    //m_user->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    
    m_phonenum = new QLabel();
    m_presencevalue = new QComboBox();
    m_presencevalue->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_presencevalue->setProperty("function", "presence");
    
    m_phone = new QLabel();
    m_phonestatustxt = new QLabel();
    m_phonestatustxt->setText("");
    m_phonestatustxt->setScaledContents(true);
    m_phonestatustxt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_phonecall = new QLabel();
    m_phonecalltxt = new QLabel();
    
    m_voicemail_old = new QLabel();
    m_voicemail_new = new QLabel();
    m_voicemail_name = new QLabel();
    
    m_agent = new QLabel();
    m_agentstatus = new QLabel();
    m_agentstatustxt = new QLabel();
    m_agentpause = new QLabel();
    m_agentpausetxt = new QLabel();
    
    connect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(idxChanged(const QString &)));
    
    m_icon_user = new ExtendedLabel();
    m_icon_agent = new ExtendedLabel();
    m_icon_phone = new ExtendedLabel();
    m_icon_voicemail = new ExtendedLabel();
    m_icon_voicemail->hide();
    
    m_icon_user->setPixmap(QPixmap(":/images/personal.png"));
    m_icon_agent->setPixmap(QPixmap(":/images/applixware.png"));
    m_icon_phone->setPixmap(QPixmap(":/images/phone.png"));
    m_icon_voicemail->setPixmap(QPixmap(":/images/kthememgr.png"));
    
    m_icon_user->setProperty("iconname", "user");
    m_icon_agent->setProperty("iconname", "agent");
    m_icon_phone->setProperty("iconname", "phone");
    m_icon_voicemail->setProperty("iconname", "voicemail");
    
    connect( m_icon_user, SIGNAL(context_menu(QContextMenuEvent *)),
             this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
    connect( m_icon_agent, SIGNAL(context_menu(QContextMenuEvent *)),
             this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
    connect( m_icon_phone, SIGNAL(context_menu(QContextMenuEvent *)),
             this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
    connect( m_icon_voicemail, SIGNAL(context_menu(QContextMenuEvent *)),
             this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
    
    m_glayout->setSpacing(2);
    // m_glayout->setMargin(0);
    
    m_col_user = 0;
    m_col_agent = 2;
    m_col_phone = 5;
    m_col_vm = 8;
    m_col_last = 10;
    
    m_iconAlign = Qt::AlignHCenter | Qt::AlignTop; // Qt::AlignVCenter
    m_textAlignVCenter = Qt::AlignLeft | Qt::AlignVCenter;
    // m_textAlignTop = Qt::AlignLeft | Qt::AlignTop;
    
    setupIcons();
    
    // although it might be convenient in some cases (prevent some expansions),
    // in the basic xivoclient/grid case, it fills too much room without no resizing available
    //m_glayout->setRowStretch( idline, 1 );
    
    hideAgentProps();
    
    setGuiOptions(options);
    //         m_glayout->setColumnStretch( 0, 1 );
}

void IdentityDisplay::setupIcons()
{
    m_glayout->addWidget( m_icon_user, 0, m_col_user, 3, 1, m_iconAlign );
    m_glayout->addWidget( m_icon_agent, 0, m_col_agent, 3, 1, m_iconAlign );
    m_glayout->addWidget( m_icon_phone, 0, m_col_phone, 3, 1, m_iconAlign );
    m_glayout->addWidget( m_icon_voicemail, 0, m_col_vm, 3, 1, m_iconAlign );
    int idline = 0;
    m_glayout->addWidget( m_user, idline, m_col_user + 1, m_textAlignVCenter );
    m_glayout->addWidget( m_agent, idline, m_col_agent + 1, 1, 2, m_textAlignVCenter );
    m_glayout->addWidget( m_phone, idline, m_col_phone + 1, 1, 2, m_textAlignVCenter );
    m_glayout->addWidget( m_voicemail_name, idline, m_col_vm + 1, m_textAlignVCenter );
    idline ++;
    m_glayout->addWidget( m_phonenum, idline, m_col_user + 1, m_textAlignVCenter );
    m_glayout->addWidget( m_agentstatus, idline, m_col_agent + 1, m_textAlignVCenter );
    m_glayout->addWidget( m_agentstatustxt, idline, m_col_agent + 2, m_textAlignVCenter );
    m_glayout->addWidget( m_phonecall, idline, m_col_phone + 1, m_textAlignVCenter );
    m_glayout->addWidget( m_phonecalltxt, idline, m_col_phone + 2, m_textAlignVCenter );
    m_glayout->addWidget( m_voicemail_old, idline, m_col_vm + 1, m_textAlignVCenter );
    idline ++;
    m_glayout->addWidget( m_presencevalue, idline, m_col_user + 1, m_textAlignVCenter );
    m_glayout->addWidget( m_agentpause, idline, m_col_agent + 1, m_textAlignVCenter );
    m_glayout->addWidget( m_agentpausetxt, idline, m_col_agent + 2, m_textAlignVCenter );
    m_glayout->addWidget( m_phonestatustxt, idline, m_col_phone + 1, 1, 2, m_textAlignVCenter );
    m_glayout->addWidget( m_voicemail_new, idline, m_col_vm + 1, m_textAlignVCenter );
    for(int i = 0; i < m_col_last; i++)
        if(m_glayout->columnStretch(i) == 1)
            m_glayout->setColumnStretch(i, 0);
    m_glayout->setColumnStretch( m_col_last, 1 );
}

void IdentityDisplay::setGuiOptions(const QVariant & options)
{
    // qDebug() << "IdentityDisplay::setGuiOptions()" << options;
    QVariantMap optionsMap = options.toMap();
    if(optionsMap.contains("fontname") && optionsMap.contains("fontsize"))
        m_gui_font = QFont(optionsMap["fontname"].toString(),
                           optionsMap["fontsize"].toInt());
    if(optionsMap.contains("iconsize"))
        m_gui_buttonsize = optionsMap["iconsize"].toInt();
    
    m_loginkind = optionsMap["loginkind"].toUInt();
    m_functions = optionsMap["functions"].toStringList();
    
    m_allow_logagent = optionsMap["logagent"].toBool();
    m_allow_pauseagent = optionsMap["pauseagent"].toBool();
    
    setFont(m_gui_font);
}

void IdentityDisplay::contextMenuEvent(QContextMenuEvent * event)
{
    // qDebug() << "IdentityDisplay::contextMenuEvent()";
    if(sender() != NULL) {
        QString iconname = sender()->property("iconname").toString();
        // QMenu contextMenu(this);
        if(iconname == "agent") {
            QMenu contextMenu(this);
            
            if(m_allow_logagent) {
                QAction * logAction = new QAction(this);
                bool connected = m_agentstatustxt->property("connected").toBool();
                if(connected)
                    logAction->setText(tr("Logout"));
                else
                    logAction->setText(tr("Login"));
                logAction->setProperty("iconname", iconname);
                logAction->setProperty("kind", "log");
                connect(logAction, SIGNAL(triggered()),
                        this, SLOT(contextMenuAction()) );
                contextMenu.addAction(logAction);
            }
            
            if(m_allow_pauseagent) {
                QAction * pauseAction = new QAction(tr("Pause"), this);
                pauseAction->setProperty("iconname", iconname);
                pauseAction->setProperty("kind", "pause");
                connect(pauseAction, SIGNAL(triggered()),
                        this, SLOT(contextMenuAction()) );
                contextMenu.addAction(pauseAction);
                
                QAction * unpauseAction = new QAction(tr("Unpause"), this);
                unpauseAction->setProperty("iconname", iconname);
                unpauseAction->setProperty("kind", "unpause");
                connect(unpauseAction, SIGNAL(triggered()),
                        this, SLOT(contextMenuAction()) );
                contextMenu.addAction(unpauseAction);
            }
            
            if(m_allow_logagent || m_allow_pauseagent)
                contextMenu.exec(event->globalPos());
            //                 } else if(iconname == "user") {
            //                         QAction * xAction = new QAction(tr("Change State"), this);
            //                         connect(xAction, SIGNAL(triggered()),
            //                                 this, SLOT(contextMenuAction()) );
            //                         contextMenu.addAction(xAction);
            //                 } else if(iconname == "voicemail") {
            //                         QAction * xAction = new QAction(tr("Listen Message"), this);
            //                         connect(xAction, SIGNAL(triggered()),
            //                                 this, SLOT(contextMenuAction()) );
            //                         contextMenu.addAction(xAction);
//         } else if(iconname == "phone") {
//             QMenu contextMenu(this);
//             QAction * a1 = new QAction("dnd", this);
//             contextMenu.addAction(a1);
//             QAction * a2 = new QAction("fwd", this);
//             contextMenu.addAction(a2);
//             contextMenu.exec(event->globalPos());
        } else if(iconname == "phone") {
            foreach(QString uniqueid, m_comms.keys()) {
                QString thiscommchan = m_comms[uniqueid].toMap()["thischannel"].toString();
                if(! thiscommchan.isEmpty()) {
                    QMenu contextMenu(this);
                    QAction * hangupMe = new QAction(tr("Cancel"), this);
                    hangupMe->setProperty("iconname", iconname);
                    hangupMe->setProperty("channel", thiscommchan);
                    connect(hangupMe, SIGNAL(triggered()),
                            this, SLOT(contextMenuAction()) );
                    contextMenu.addAction(hangupMe);
                    contextMenu.exec(event->globalPos());
                }
            }
        } else if(iconname == "phoneline") {
            QString linenumber = sender()->property("linenumber").toString();
            qDebug() << "IdentityDisplay::contextMenuEvent()" << iconname << linenumber;
        }
    }
}

void IdentityDisplay::contextMenuAction()
{
    QString iconname = sender()->property("iconname").toString();
    if(iconname == "agent") {
        QString kind = sender()->property("kind").toString();
        if(kind == "log")
            doAgentLogActions();
        else if(kind == "pause")
            doAgentPauseActions();
        else if(kind == "unpause")
            doAgentUnPauseActions();
    } else if(iconname == "phone") {
        actionCall("hangup", QString("chan:%1:%2").arg(m_ui->userid()).arg(sender()->property("channel").toString()));
        // qDebug() << m_ui->astid() << sender()->property("channel").toString();
    }
}

void IdentityDisplay::updatePresence(const QVariant & presence)
{
    // qDebug() << "IdentityDisplay::updatePresence()" << presence;
    m_presencevalue->hide();
    if(! m_functions.contains("presence"))
        return;
    
    disconnect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
               this, SLOT(idxChanged(const QString &)));
    QVariantMap presencemap = presence.toMap();
    if(presencemap.contains("names")) {
        foreach (QString avstate, presencemap["names"].toMap().keys()) {
            QString name = presencemap["names"].toMap()[avstate].toMap()["longname"].toString();
            if(m_presencevalue->findText(name) == -1) {
                m_presencevalue->addItem(name);
                m_presence_names[avstate] = name;
            }
        }
    }
    if(presencemap.contains("allowed")) {
        QMapIterator<QString, QVariant> capapres(presencemap["allowed"].toMap());
        while (capapres.hasNext()) {
            capapres.next();
            QString avstate = capapres.key();
            bool allow = capapres.value().toBool();
            if(m_presence_names.contains(avstate)) {
                QString name = m_presence_names[avstate];
                int idx = m_presencevalue->findText(name);
                if(idx != -1) {
                    if(! allow)
                        m_presencevalue->removeItem(idx);
                }
            }
        }
    }
    if(presencemap.contains("state")) {
        QString avstate = presencemap["state"].toMap()["stateid"].toString();
        if(m_presence_names.contains(avstate)) {
            QString name = m_presence_names[avstate];
            int idx = m_presencevalue->findText(name);
            m_presencevalue->setCurrentIndex(idx);
        }
    }
    connect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(idxChanged(const QString &)));
    m_presencevalue->show();
}

/*! \brief display the phones' lines
 */
void IdentityDisplay::setPhoneLines()
{
    QPixmap * p_square = new QPixmap(6, 6);
    p_square->fill(Qt::black);
    for(int jj = 0 ; jj < m_nlines ; jj ++) {
        QString sjj = QString::number(jj + 1);
        if(! m_lineaction.contains(sjj)) {
            m_lineaction[sjj] = new ExtendedLabel();
            m_lineaction[sjj]->setProperty("iconname", "phoneline");
            m_lineaction[sjj]->setProperty("linenumber", sjj);
            connect( m_lineaction[sjj], SIGNAL(context_menu(QContextMenuEvent *)),
                     this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
            
            m_linestatus[sjj] = new ExtendedLabel();
            m_linestatus[sjj]->setProperty("iconname", "phoneline");
            m_linestatus[sjj]->setProperty("linenumber", sjj);
            connect( m_linestatus[sjj], SIGNAL(context_menu(QContextMenuEvent *)),
                     this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
        }
        m_lineaction[sjj]->setPixmap(* p_square);
        
        int ix = jj / 3;
        int iy = jj % 3;
        m_glayout->addWidget( m_lineaction[sjj], iy, m_col_phone + 3 + 2 * ix, m_textAlignVCenter );
        m_glayout->addWidget( m_linestatus[sjj], iy, m_col_phone + 4 + 2 * ix, m_textAlignVCenter );
    }
    
    m_col_vm = 8 + 2 * ((m_nlines + 2) / 3);
    m_col_last = 10 + 2 * ((m_nlines + 2) / 3);
    setupIcons();
}

void IdentityDisplay::setUserInfo(const UserInfo * ui)
{
    // qDebug() << "IdentityDisplay::setUserInfo()";
    m_ui = ui;
    
    m_user->setText(m_ui->fullname());
    m_phonenum->setText(m_ui->phonenumber());
    m_phone->setText(tr("Phone %1").arg(m_ui->phonenumber()));
    m_nlines = m_ui->simultcalls();
    setPhoneLines();
    QStringList vm = m_ui->mwi();
    if(vm.size() > 2) {
        m_icon_voicemail->show();
        m_voicemail_old->setText(tr("%1 old").arg(vm[1]));
        m_voicemail_new->setText(tr("%1 new").arg(vm[2]));
        m_voicemail_name->setText(tr("VoiceMailBox %1").arg(m_ui->voicemailnumber()));
    }
    // m_voicemail->hide();
    // changes the "watched agent" only if no one else has done it before
    changeWatchedAgent(QString("agent:%1/%2").arg(m_ui->astid()).arg(m_ui->agentid()), false);
}

/*! \brief slot when one or more agents have been updated
 */
void IdentityDisplay::newAgentList(const QStringList &)
{
    // qDebug() << "IdentityDisplay::newAgentList()" << m_loginkind << list << m_engine->agents();
    if (m_loginkind == 0 || ! m_ui)
        return;
    QHashIterator<QString, AgentInfo *> iter = QHashIterator<QString, AgentInfo *>(m_engine->agents());
    while( iter.hasNext() )
    {
        iter.next();
        AgentInfo * ainfo = iter.value();
        QString agentid = iter.key();
        // qDebug() << "IdentityDisplay::newAgentList" << m_ui->astid() << ainfo->astid() << m_ui->agentnumber() << ainfo->agentnumber();
        if((m_ui->astid() == ainfo->astid()) && (m_ui->agentnumber() == ainfo->agentnumber())) {
            m_agent->setText(QString("Agent %1").arg(ainfo->agentnumber()));
            showAgentProps();
            updateAgentStatus(ainfo->properties());
        }
    }
}

/*! \brief slot when one or more queues have been updated
 */
void IdentityDisplay::newQueueList(const QStringList &)
{
/*
    if (m_loginkind == 0)
        return;
    if (m_ui == NULL)
        return;
*/
    // qDebug() << "IdentityDisplay::newQueueList()";
}

/*! \brief updates the boolean services
 */
void IdentityDisplay::setOpt(const QString & capa, bool b)
{
    if((capa == "enablednd") || (capa == "incallfilter") || (capa == "incallrec") || (capa == "enablevm"))
        m_svcstatus[capa] = b;
    svcSummary();
}

/*! \brief updates the boolean+value services
 */
void IdentityDisplay::setForward(const QString & capa, const QVariant & value)
{
    if((capa == "unc") || (capa == "busy") || (capa == "rna")) {
        m_svcstatus[capa + "-enabled"] = value.toMap()["enabled"];
        m_svcstatus[capa + "-number"] = value.toMap()["number"];
    }
    svcSummary();
}

/*! \brief updates the display of Services data
 */
void IdentityDisplay::svcSummary()
{
    if(m_svcstatus["enablednd"].toBool()) {
        m_phonestatustxt->setText(tr("DND"));
        m_phonestatustxt->setToolTip(tr("Do Not Disturb"));
    } else {
        if(m_svcstatus["unc-enabled"].toBool()) {
            m_phonestatustxt->setText(tr("UNC %1").arg(m_svcstatus["unc-number"].toString()));
            m_phonestatustxt->setToolTip(tr("Unconditional Forward towards %1").arg(m_svcstatus["unc-number"].toString()));
        } else if (m_svcstatus["busy-enabled"].toBool()) {
            m_phonestatustxt->setText(tr("Busy %1").arg(m_svcstatus["busy-number"].toString()));
            m_phonestatustxt->setToolTip(tr("Busy Forward towards %1").arg(m_svcstatus["busy-number"].toString()));
        } else if (m_svcstatus["rna-enabled"].toBool()) {
            m_phonestatustxt->setText(tr("RNA %1").arg(m_svcstatus["rna-number"].toString()));
            m_phonestatustxt->setToolTip(tr("Non-Answer Forward towards %1").arg(m_svcstatus["rna-number"].toString()));
        } else if (m_svcstatus["incallrec"].toBool()) {
            m_phonestatustxt->setText("Call Rec");
        } else if (m_svcstatus["incallfilter"].toBool()) {
            m_phonestatustxt->setText("Call Filter");
        } else {
            m_phonestatustxt->setText("");
            m_phonestatustxt->setToolTip("");
        }
    }
    return;
}

/*! \brief update user status
 */
void IdentityDisplay::updateUser(UserInfo * ui)
{
    if(m_ui != ui)
        return;
    foreach(QString phoneid, m_ui->phonelist()) {
        QPixmap * p_square = new QPixmap(10, 10);
        const PhoneInfo * p_pi = m_ui->getPhoneInfo(phoneid);
        p_square->fill(p_pi->hintstatus("color"));
        m_phonecall->setPixmap(* p_square);
        m_phonecall->setToolTip(p_pi->hintstatus("longname"));
        m_phonecalltxt->setText(p_pi->hintstatus("longname"));
        m_comms = p_pi->comms();
        int ic = 0;
        foreach(QString callref, m_comms.keys()) {
            QString status = m_comms[callref].toMap()["status"].toString();
            QString todisplay = m_comms[callref].toMap()["calleridname"].toString();
            QPixmap * p_square_comm = new QPixmap(6, 6);
            p_square_comm->fill(Qt::green);
            if(status == "hangup") {
                todisplay = "";
                p_square_comm->fill(Qt::black);
            }
            m_lineaction[QString::number(ic + 1)]->setPixmap(* p_square_comm);
            m_linestatus[QString::number(ic + 1)]->setText(todisplay);
            // qDebug() << "IdentityDisplay::updateUser" << phoneid << callref << m_comms[callref];
            ic ++;
        }
    }
}

void IdentityDisplay::updateAgentStatus(const QVariantMap & properties)
{
    QVariantMap agqjoined = properties["queues_by_agent"].toMap();
    QVariantMap agentstats =  properties["agentstats"].toMap();
    QString agstatus = agentstats["status"].toString();
    QString phonenum = agentstats["agent_phone_number"].toString();
    
    if(agstatus != m_agstatus) {
        m_agstatus = agstatus;
        if(agstatus == "AGENT_LOGGEDOFF") {
            setSystrayIcon(icon_color_black);
            m_agentstatustxt->setProperty("connected", false);
            setStatusColors(phonenum);
        } else if(agstatus == "AGENT_IDLE") {
            setSystrayIcon(icon_color_green);
            m_agentstatustxt->setProperty("connected", true);
            setStatusColors(phonenum);
        } else if(agstatus == "AGENT_ONCALL") {
            setSystrayIcon(icon_color_green);
            m_agentstatustxt->setProperty("connected", true);
            setStatusColors(phonenum);
        } else
            qDebug() << "IdentityDisplay::setAgentList() unknown status" << agstatus;
    }
    
    QStringList joined_queues;
    QStringList unpaused_queues;
    foreach (QString qname, agqjoined.keys()) {
        QVariantMap qv = agqjoined[qname].toMap();
        if(qv.contains("Status")) {
            QString pstatus = qv["Paused"].toString();
            //QString sstatus = qv["Status"].toString();
            joined_queues << qname;
            if(pstatus == "0")
                unpaused_queues << qname;
        }
    }
    
    int njoined = joined_queues.size();
    int nunpaused = unpaused_queues.size();
    setPausedColors(njoined, njoined - nunpaused);
}

void IdentityDisplay::showAgentProps()
{
    m_agent->show();
    m_agentstatus->show();
    m_agentstatustxt->show();
    m_agentpause->show();
    m_agentpausetxt->show();
    m_icon_agent->show();
}

void IdentityDisplay::hideAgentProps()
{
    m_agent->hide();
    m_agentstatus->hide();
    m_agentstatustxt->hide();
    m_agentpause->hide();
    m_agentpausetxt->hide();
    m_icon_agent->hide();
}

void IdentityDisplay::setStatusColors(const QString & phonenum)
{
    QPixmap * p_square = new QPixmap(10, 10);
    if(m_agentstatustxt->property("connected").toBool()) {
        p_square->fill("#00ff00");
        m_agentstatustxt->setText(tr("Connected on %1").arg(phonenum));
    } else {
        p_square->fill("#ff0000");
        m_agentstatustxt->setText(tr("Disconnected from %1").arg(phonenum));
    }
    m_agentstatus->setPixmap(* p_square);
}

void IdentityDisplay::setPausedColors(int nj, int np)
{
    QPixmap * p_square = new QPixmap(10, 10);
    if(nj > 0) {
        if(np == nj) {
            setSystrayIcon(icon_color_red);
            p_square->fill("#ff0000");
            m_agentpause->setToolTip(tr("Paused"));
            m_agentpausetxt->setText(tr("Paused"));
        } else if(np == 0) {
            bool loggedin = m_agentstatustxt->property("connected").toBool();
            p_square->fill("#00ff00");
            m_agentpause->setToolTip(tr("Unpaused"));
            m_agentpausetxt->setText(tr("Unpaused"));
            if(loggedin)
                setSystrayIcon(icon_color_green);
            else
                setSystrayIcon(icon_color_black);
        } else {
            p_square->fill(Orange);
            m_agentpause->setToolTip(tr("Partially paused"));
            m_agentpausetxt->setText(tr("Partially paused"));
        }
    } else {
        p_square->fill(Qt::gray);
        m_agentpause->setToolTip(tr("Not relevant"));
        m_agentpausetxt->setText(tr("Not relevant"));
    }
    m_agentpause->setPixmap(* p_square);
}

void IdentityDisplay::doAgentLogActions()
{
    bool connected = m_agentstatustxt->property("connected").toBool();
    if(connected)
        agentAction("logout");
    else
        agentAction("login");
}

void IdentityDisplay::doAgentPauseActions()
{
    // bool paused = m_agentpause->property("paused").toBool();
    agentAction("pause_all");
}

void IdentityDisplay::doAgentUnPauseActions()
{
    // bool paused = m_agentpause->property("paused").toBool();
    agentAction("unpause_all");
}

void IdentityDisplay::idxChanged(const QString & newidx)
{
    QString function = sender()->property("function").toString();
    // qDebug() << "IdentityDisplay::idxChanged" << newidx << sender() << function;
    if(function == "presence") {
        foreach (QString avstate, m_presence_names.keys())
            if(m_presence_names[avstate] == newidx)
                setAvailState(avstate, false);
    }
}
