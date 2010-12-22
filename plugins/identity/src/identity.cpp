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

    m_agent->hide();

    m_functions = b_engine->getGuiOptions("server_funcs").value("functions").toStringList();
    setGuiOptions(b_engine->getGuiOptions("merged_gui"));

    // connect signals/slots
    connect(b_engine, SIGNAL(newAgentList(const QStringList &)),
            this, SLOT(newAgentList(const QStringList &)));

    connect(b_engine, SIGNAL(updatePresence(const QVariant &)),
            this, SLOT(updatePresence(const QVariant &)));
    connect(this, SIGNAL(setAvailState(const QString &, bool)),
            b_engine, SLOT(setAvailState(const QString &, bool)));
    connect(this, SIGNAL(changeWatchedAgent(const QString &, bool)),
            b_engine, SLOT(changeWatchedAgentSlot(const QString &, bool)));

    connect(b_engine, SIGNAL(optChanged(const QString &, bool)),
            this, SLOT(setOpt(const QString &, bool)));
    connect(b_engine, SIGNAL(forwardUpdated(const QString &, const QVariant &)),
            this, SLOT(setForward(const QString &, const QVariant &)));
    connect(b_engine, SIGNAL(userUpdated(UserInfo *)),
            this, SLOT(updateUser(UserInfo *)));
    connect(b_engine, SIGNAL(userUpdated(UserInfo *)),
            m_phone, SLOT(updateUser(UserInfo *)));

    connect(b_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
            this, SLOT(setUserInfo(const UserInfo *)));
    connect(b_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
            m_phone, SLOT(setUserInfo(const UserInfo *)));
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
    if(optionsMap.contains("xlet.identity.fontname") && optionsMap.contains("xlet.identity.fontsize"))
        m_gui_font = QFont(optionsMap.value("xlet.identity.fontname").toString(),
                           optionsMap.value("xlet.identity.fontsize").toInt());
    if(optionsMap.contains("xlet.identity.iconsize"))
        m_gui_buttonsize = optionsMap.value("xlet.identity.iconsize").toInt();

    m_agent->setAllowedActions(optionsMap.value("xlet.identity.logagent").toBool(),
                               optionsMap.value("xlet.identity.pauseagent").toBool());

    setFont(m_gui_font);

    m_loginkind = optionsMap.value("loginkind").toUInt();
}

void IdentityDisplay::updatePresence(const QVariant & presence)
{
    // qDebug() << Q_FUNC_INFO << presence;
    m_presencevalue->hide();
    if(! m_functions.contains("presence"))
        return;

    disconnect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
               this, SLOT(idxChanged(const QString &)));
    QVariantMap presencemap = presence.toMap();
    if(presencemap.contains("names")) {
        foreach (QString avstate, presencemap.value("names").toMap().keys()) {
            QString name = presencemap.value("names").toMap().value(avstate).toMap().value("longname").toString();
            if(m_presencevalue->findText(name) == -1) {
                m_presencevalue->addItem(name);
                m_presence_names[avstate] = name;
            }
        }
    }
    if(presencemap.contains("allowed")) {
        QMapIterator<QString, QVariant> capapres(presencemap.value("allowed").toMap());
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
        QString avstate = presencemap.value("state").toMap().value("stateid").toString();
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

void IdentityDisplay::setUserInfo(const UserInfo */* ui */)
{
    // qDebug() << Q_FUNC_INFO;
    m_ui = b_engine->getXivoClientUser();

    m_user->setText(m_ui->fullname());
    m_phonenum->setText(m_ui->phoneNumber());
    m_phonenum->setToolTip(tr("Server: %1\nContext: %2")
                           .arg(m_ui->astid())
                           .arg(m_ui->context()));
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
    // qDebug() << Q_FUNC_INFO << m_loginkind << list << b_engine->agents();
    if (m_loginkind == 0 || ! m_ui)
        return;
    QHashIterator<QString, AgentInfo *> iter = QHashIterator<QString, AgentInfo *>(b_engine->agents());
    while( iter.hasNext() )
    {
        iter.next();
        AgentInfo * ainfo = iter.value();
        QString agentid = iter.key();
        if((m_ui->astid() == ainfo->astid()) && (m_ui->agentNumber() == ainfo->agentNumber())) {
            m_agent->setText(QString("Agent %1").arg(ainfo->agentNumber()));
            m_agent->show();
            m_agent->updateStatus(ainfo->properties());
        }
    }
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
        m_svcstatus[capa + "-enabled"] = value.toMap().value("enabled");
        m_svcstatus[capa + "-number"] = value.toMap().value("number");
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
                QString ics = callprops.value("linenum").toString();
                QString status = callprops.value("status").toString();
                QString todisplay = callprops.value("calleridname").toString();
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
    // qDebug() << Q_FUNC_INFO << newidx << sender() << function;
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
