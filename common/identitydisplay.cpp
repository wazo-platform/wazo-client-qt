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
#include "identityagent.h"

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
    
    m_phonenum = new QLabel(this);
    m_presencevalue = new QComboBox(this);
    m_presencevalue->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_presencevalue->setProperty("function", "presence");
    
    m_phone = new QLabel(this);
    m_phonestatustxt = new QLabel(tr("No option"), this);
    m_phonestatustxt->setScaledContents(true);
    m_phonestatustxt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    m_phonecall = new QLabel(this);
    m_phonecalltxt = new QLabel(this);
    
    m_phone->setContentsMargins(0, 0, 10, 0);
    m_phonecall->setContentsMargins(0, 0, 0, 0);
    m_phonecalltxt->setContentsMargins(0, 0, 10, 0);
    m_phonestatustxt->setContentsMargins(0, 0, 10, 0);
    
    m_voicemail_old = new QLabel(this);
    m_voicemail_new = new QLabel(this);
    m_voicemail_name = new QLabel(this);
    
    connect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(idxChanged(const QString &)));
    
    m_icon_user = new ExtendedLabel("", this);
    m_icon_phone = new ExtendedLabel("", this);
    m_icon_voicemail = new ExtendedLabel("", this);
    m_icon_voicemail->hide();
    
    m_icon_user->setPixmap(QPixmap(":/images/personal.png"));
    m_icon_phone->setPixmap(QPixmap(":/images/phone.png"));
    m_icon_voicemail->setPixmap(QPixmap(":/images/kthememgr.png"));
    
    m_icon_user->setContentsMargins(0, 0, 5, 0);
    m_icon_phone->setContentsMargins(20, 0, 5, 0);
    m_icon_voicemail->setContentsMargins(20, 0, 5, 0);
    
//     m_icon_user->setFrameStyle(QFrame::Panel | QFrame::Raised);
//     m_icon_user->setLineWidth(2);
//     m_icon_agent->setFrameStyle(QFrame::Panel | QFrame::Raised);
//     m_icon_agent->setLineWidth(2);
//     m_icon_phone->setFrameStyle(QFrame::Panel | QFrame::Raised);
//     m_icon_phone->setLineWidth(2);
//     m_icon_voicemail->setFrameStyle(QFrame::Panel | QFrame::Raised);
//     m_icon_voicemail->setLineWidth(2);
    
    m_icon_user->setProperty("iconname", "user");
    m_icon_phone->setProperty("iconname", "phone");
    m_icon_voicemail->setProperty("iconname", "voicemail");
    
    connect( m_icon_user, SIGNAL(context_menu(QContextMenuEvent *)),
             this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
    connect( m_icon_phone, SIGNAL(context_menu(QContextMenuEvent *)),
             this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
    connect( m_icon_voicemail, SIGNAL(context_menu(QContextMenuEvent *)),
             this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
    
    m_agent = new IdentityAgent(this);
    connect( m_agent, SIGNAL(agentAction(const QString &)),
             m_engine, SLOT(agentAction(const QString &)) );
    connect( m_agent, SIGNAL(setSystrayIcon(const QString &)),
             this, SIGNAL(setSystrayIcon(const QString &)) );
    m_agent->setContentsMargins(20, 0, 0, 0);
    
    m_glayout->setSpacing(0);
    m_glayout->setMargin(0);
    
    m_col_user = 0;
    m_col_agent = 2;
    m_col_phone = 3;
    m_col_vm = 6;
    m_col_last = 8;
    
    m_iconAlign = Qt::AlignHCenter | Qt::AlignTop; // Qt::AlignVCenter
    m_textAlignVCenter = Qt::AlignLeft | Qt::AlignVCenter;
    // m_textAlignTop = Qt::AlignLeft | Qt::AlignTop;
    
    setupIcons();
    m_glayout->addWidget( m_agent, 0, m_col_agent, 3, 1 );
    
    // although it might be convenient in some cases (prevent some expansions),
    // in the basic xivoclient/grid case, it fills too much room without no resizing available
    //m_glayout->setRowStretch( idline, 1 );
    
    m_agent->hide();
    
    setGuiOptions(options);
    //         m_glayout->setColumnStretch( 0, 1 );
}

void IdentityDisplay::setupIcons()
{
    m_glayout->addWidget( m_icon_user, 0, m_col_user, 3, 1, m_iconAlign );
    m_glayout->addWidget( m_icon_phone, 0, m_col_phone, 3, 1, m_iconAlign );
    m_glayout->addWidget( m_icon_voicemail, 0, m_col_vm, 3, 1, m_iconAlign );
    int idline = 0;
    m_glayout->addWidget( m_user, idline, m_col_user + 1, m_textAlignVCenter );
    m_glayout->addWidget( m_phone, idline, m_col_phone + 1, 1, 2, m_textAlignVCenter );
    m_glayout->addWidget( m_voicemail_name, idline, m_col_vm + 1, m_textAlignVCenter );
    idline ++;
    m_glayout->addWidget( m_phonenum, idline, m_col_user + 1, m_textAlignVCenter );
    m_glayout->addWidget( m_phonecall, idline, m_col_phone + 1, m_textAlignVCenter );
    m_glayout->addWidget( m_phonecalltxt, idline, m_col_phone + 2, m_textAlignVCenter );
    m_glayout->addWidget( m_voicemail_old, idline, m_col_vm + 1, m_textAlignVCenter );
    idline ++;
    m_glayout->addWidget( m_presencevalue, idline, m_col_user + 1, m_textAlignVCenter );
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
    
    m_agent->setAllowedActions( optionsMap["logagent"].toBool(), optionsMap["pauseagent"].toBool() );
    
    setFont(m_gui_font);
}

void IdentityDisplay::contextMenuEvent(QContextMenuEvent * event)
{
    qDebug() << "IdentityDisplay::contextMenuEvent()" << sender();
    if(sender() != NULL) {
        QString iconname = sender()->property("iconname").toString();
        if(iconname == "phoneline") {
            int linenumber = sender()->property("linenumber").toInt();
            QMapIterator<QString, QVariant> iter = QMapIterator<QString, QVariant>(m_comms);
            while( iter.hasNext() ) {
                iter.next();
                QVariantMap callprops = iter.value().toMap();
                if(callprops.contains("linenum")) {
                    int ic = callprops["linenum"].toInt();
                    if(ic == linenumber) {
                        QString thiscommchan = callprops["thischannel"].toString();
                        if(! thiscommchan.isEmpty()) {
                            QMenu contextMenu(this);
                            
                            QAction * hangupMe = new QAction(tr("Hangup"), &contextMenu);
                            hangupMe->setProperty("iconname", iconname);
                            hangupMe->setProperty("channel", thiscommchan);
                            hangupMe->setProperty("action", "hangup");
                            connect(hangupMe, SIGNAL(triggered()),
                                    this, SLOT(contextMenuAction()) );
                            contextMenu.addAction(hangupMe);
                            
                            QAction * replyMe = new QAction(tr("Reply"), &contextMenu);
                            replyMe->setProperty("iconname", iconname);
                            replyMe->setProperty("channel", thiscommchan);
                            replyMe->setProperty("action", "pickup");
                            connect(replyMe, SIGNAL(triggered()),
                                    this, SLOT(contextMenuAction()) );
                            contextMenu.addAction(replyMe);
                            
                            QAction * refuseMe = new QAction(tr("Refuse"), &contextMenu);
                            refuseMe->setProperty("iconname", iconname);
                            refuseMe->setProperty("channel", thiscommchan);
                            refuseMe->setProperty("action", "refuse");
                            connect(refuseMe, SIGNAL(triggered()),
                                    this, SLOT(contextMenuAction()) );
                            contextMenu.addAction(refuseMe);
                            
                            contextMenu.exec(event->globalPos());
                        }
                        break;
                    }
                }
            }
        }
    }
}

void IdentityDisplay::contextMenuAction()
{
    QString iconname = sender()->property("iconname").toString();
    QString actionname = sender()->property("action").toString();
    if(iconname == "phoneline")
        actionCall(actionname, QString("chan:%1:%2").arg(m_ui->userid()).arg(sender()->property("channel").toString()));
    // qDebug() << m_ui->astid() << sender()->property("channel").toString();
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
    QPixmap square(25, 3);
    square.fill(Qt::black);
    for(int jj = 0 ; jj < m_nlines ; jj ++) {
        QString sjj = QString::number(jj + 1);
        if(! m_lineaction.contains(sjj)) {
            m_lineaction[sjj] = new ExtendedLabel("", this);
            m_lineaction[sjj]->setProperty("iconname", "phoneline");
            m_lineaction[sjj]->setProperty("linenumber", jj + 1);
            connect( m_lineaction[sjj], SIGNAL(context_menu(QContextMenuEvent *)),
                     this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
            
            m_linestatus[sjj] = new ExtendedLabel("", this);
            m_linestatus[sjj]->setProperty("iconname", "phoneline");
            m_linestatus[sjj]->setProperty("linenumber", jj + 1);
            connect( m_linestatus[sjj], SIGNAL(context_menu(QContextMenuEvent *)),
                     this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
            
            m_lineaction[sjj]->setPixmap(square);
            QString todisplay = tr("(Line %1)").arg(jj + 1);
            m_linestatus[sjj]->setText(QString("  %1  ").arg(todisplay));
        }
        
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
        if(m_svcstatus["enablevm"].toBool()) {
            m_voicemail_name->setText(tr("<b>VoiceMailBox %1</b>").arg(m_ui->voicemailnumber()));
            m_voicemail_name->setToolTip(tr("VoiceMail activated on %1").arg(m_ui->voicemailnumber()));
        } else {
            m_voicemail_name->setText(tr("VoiceMailBox %1").arg(m_ui->voicemailnumber()));
            m_voicemail_name->setToolTip(tr("VoiceMail not activated on %1").arg(m_ui->voicemailnumber()));
        }
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
            m_agent->show();
            m_agent->updateStatus(ainfo->properties());
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
            m_phonestatustxt->setText(tr("Call Rec"));
        } else if (m_svcstatus["incallfilter"].toBool()) {
            m_phonestatustxt->setText(tr("Call Filter"));
        } else {
            m_phonestatustxt->setText(tr("No option"));
            m_phonestatustxt->setToolTip(tr("No option"));
        }
    }
    QStringList vm = m_ui->mwi();
    if(vm.size() > 2) {
        if(m_svcstatus["enablevm"].toBool()) {
            m_voicemail_name->setText(tr("<b>VoiceMailBox %1</b>").arg(m_ui->voicemailnumber()));
            m_voicemail_name->setToolTip(tr("VoiceMail activated on %1").arg(m_ui->voicemailnumber()));
        } else {
            m_voicemail_name->setText(tr("VoiceMailBox %1").arg(m_ui->voicemailnumber()));
            m_voicemail_name->setToolTip(tr("VoiceMail not activated on %1").arg(m_ui->voicemailnumber()));
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
        QPixmap square(10, 10);
        const PhoneInfo * p_pi = m_ui->getPhoneInfo(phoneid);
        if(p_pi == NULL)
            continue;
        square.fill(p_pi->hintstatus("color"));
        m_phonecall->setPixmap(square);
        m_phonecall->setToolTip(p_pi->hintstatus("longname"));
        m_phonecalltxt->setText(p_pi->hintstatus("longname"));
        m_comms = p_pi->comms();
        QMapIterator<QString, QVariant> iter = QMapIterator<QString, QVariant>(m_comms);
        while( iter.hasNext() ) {
            iter.next();
            QVariantMap callprops = iter.value().toMap();
            if(callprops.contains("linenum")) {
                QString ics = callprops["linenum"].toString();
                QString status = callprops["status"].toString();
                QString todisplay = callprops["calleridname"].toString();
                bool isholded = callprops.contains("time-hold");
                
                QPixmap square_comm(25, 3);
                square_comm.fill(isholded ? Qt::darkGreen : Qt::green);
                if(status == "hangup") {
                    todisplay = tr("(Line %1)").arg(ics);
                    square_comm.fill(Qt::black);
                }
                //qDebug() << "IdentityDisplay::updateUser" << ics << m_lineaction << m_linestatus;
                if(m_lineaction.contains(ics) && m_lineaction[ics])
                    m_lineaction[ics]->setPixmap(square_comm);
                if(m_linestatus.contains(ics) && m_linestatus[ics])
                    m_linestatus[ics]->setText(QString("  %1  ").arg(todisplay));
            }
        }
    }
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

