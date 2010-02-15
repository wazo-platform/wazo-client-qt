/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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
#include "identitydisplay.h"
#include "userinfo.h"
#include "phoneinfo.h"
#include "agentinfo.h"
#include "queueinfo.h"
#include "identityagent.h"
#include "identityphone.h"
#include "identityvoicemail.h"

/*! \brief Constructor
 */
IdentityDisplay::IdentityDisplay(BaseEngine * engine,
                                 QWidget * parent)
    : XLet(engine, parent),
      m_ui(NULL)
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
    
    m_icon_user->setPixmap(QPixmap(":/images/personal.png"));
    
    m_icon_user->setContentsMargins(0, 0, 5, 0);
    
    m_agent = new IdentityAgent(this);
    connect( m_agent, SIGNAL(setSystrayIcon(const QString &)),
             this, SIGNAL(setSystrayIcon(const QString &)) );
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
    
    m_iconAlign = Qt::AlignHCenter | Qt::AlignTop; // Qt::AlignVCenter
    m_textAlignVCenter = Qt::AlignLeft | Qt::AlignVCenter;
    // m_textAlignTop = Qt::AlignLeft | Qt::AlignTop;
    
    setupIcons();
    m_glayout->addWidget( m_agent, 0, m_col_agent, 3, 1 );
    m_glayout->addWidget( m_phone, 0, m_col_phone, 3, 1 );
    m_glayout->addWidget( m_voicemail, 0, m_col_vm, 3, 1 );
    
    // although it might be convenient in some cases (prevent some expansions),
    // in the basic xivoclient/grid case, it fills too much room without no resizing available
    //m_glayout->setRowStretch( idline, 1 );
    
    m_agent->hide();
    
    m_functions = m_engine->getGuiOptions("server_funcs").value("functions").toStringList();
    setGuiOptions(m_engine->getGuiOptions("merged_gui"));
    // m_glayout->setColumnStretch( 0, 1 );

    // connect signals/slots
    connectDials();
    connect( m_engine, SIGNAL(newAgentList(const QStringList &)),
             this, SLOT(newAgentList(const QStringList &)) );
    connect( m_engine, SIGNAL(newQueueList(const QStringList &)),
             this, SLOT(newQueueList(const QStringList &)) );
    
    connect( m_engine, SIGNAL(updatePresence(const QVariant &)),
             this, SLOT(updatePresence(const QVariant &)) );
    connect( this, SIGNAL(setAvailState(const QString &, bool)),
             m_engine, SLOT(setAvailState(const QString &, bool)) );
    connect( this, SIGNAL(changeWatchedAgent(const QString &, bool)),
             m_engine, SLOT(changeWatchedAgentSlot(const QString &, bool)) );
    
    connect( m_engine, SIGNAL(optChanged(const QString &, bool)),
             this, SLOT(setOpt(const QString &, bool)) );
    connect( m_engine, SIGNAL(forwardUpdated(const QString &, const QVariant &)),
             this, SLOT(setForward(const QString &, const QVariant &)) );
    connect( m_engine, SIGNAL(userUpdated(UserInfo *)),
             this, SLOT(updateUser(UserInfo *)) );
    connect( m_engine, SIGNAL(userUpdated(UserInfo *)),
             m_phone, SLOT(updateUser(UserInfo *)) );
    connect( m_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
             this, SLOT(setUserInfo(const UserInfo *)) );
    connect( m_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
             m_phone, SLOT(setUserInfo(const UserInfo *)) );

    connect( m_phone, SIGNAL(actionCall(const QString &, const QString &, const QString &)),
             m_engine, SLOT(actionCall(const QString &, const QString &, const QString &)) );
}

void IdentityDisplay::setupIcons()
{

    m_glayout->addWidget( m_icon_user, 0, m_col_user, 3, 1, m_iconAlign );
    int idline = 0;
    m_glayout->addWidget( m_user, idline, m_col_user + 1, m_textAlignVCenter );
    idline ++;
    m_glayout->addWidget( m_phonenum, idline, m_col_user + 1, m_textAlignVCenter );
    idline ++;
    m_glayout->addWidget( m_presencevalue, idline, m_col_user + 1, m_textAlignVCenter );

    m_glayout->setColumnStretch(0, 0);
    m_glayout->setColumnStretch(1, 0);
    m_glayout->setColumnStretch(4, 1);
    /*
    for(int i = 0; i < m_col_last; i++)
        if(m_glayout->columnStretch(i) == 1)
            m_glayout->setColumnStretch(i, 0);
    m_glayout->setColumnStretch( m_col_last, 1 );
    */
}

void IdentityDisplay::setGuiOptions(const QVariantMap & optionsMap)
{
    if(optionsMap.contains("fontname") && optionsMap.contains("fontsize"))
        m_gui_font = QFont(optionsMap["fontname"].toString(),
                           optionsMap["fontsize"].toInt());
    if(optionsMap.contains("iconsize"))
        m_gui_buttonsize = optionsMap["iconsize"].toInt();
    
    m_agent->setAllowedActions( optionsMap["logagent"].toBool(), optionsMap["pauseagent"].toBool() );
    
    setFont(m_gui_font);
    
    m_loginkind = optionsMap["loginkind"].toUInt();
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

void IdentityDisplay::setUserInfo(const UserInfo * ui)
{
    // qDebug() << "IdentityDisplay::setUserInfo()";
    m_ui = ui;
    
    m_user->setText(m_ui->fullname());
    m_phonenum->setText(m_ui->phonenumber());
    QStringList vm = m_ui->mwi();
    if(vm.size() > 2) {
        m_voicemail->show();
        m_voicemail->svcSummary(m_svcstatus, m_ui);
        m_voicemail->setOldNew(vm[1], vm[2]);
    }
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
    if((capa == "enablednd") || (capa == "incallfilter") || (capa == "callrecord") || (capa == "enablevm"))
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
    if(m_phone)
        m_phone->svcSummary(m_svcstatus);
    if(m_ui) {
        QStringList vm = m_ui->mwi();
        if(vm.size() > 2) {
            m_voicemail->svcSummary(m_svcstatus, m_ui);
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
        const PhoneInfo * p_pi = m_ui->getPhoneInfo(phoneid);
        if(p_pi == NULL)
            continue;
        QMapIterator<QString, QVariant> iter = QMapIterator<QString, QVariant>(p_pi->comms());
        QStringList busylines;
        while( iter.hasNext() ) {
            iter.next();
            QVariantMap callprops = iter.value().toMap();
            if(callprops.contains("linenum")) {
                QString ics = callprops["linenum"].toString();
                QString status = callprops["status"].toString();
                QString todisplay = callprops["calleridname"].toString();
                bool isholded = callprops.contains("time-hold");
                busylines << ics;
                
                QPixmap square_comm(25, 3);
                square_comm.fill(isholded ? Qt::darkGreen : Qt::green);
                if(status == "hangup") {
                    todisplay = tr("(Line %1)").arg(ics);
                    square_comm.fill(Qt::black);
                }
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

void IdentityDisplay::doGUIConnects(QWidget * mainwindow)
{
    connect( this, SIGNAL(setSystrayIcon(const QString &)),
             mainwindow, SLOT(setSystrayIcon(const QString &)) );
}

