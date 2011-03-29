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

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QTableWidget>

#include "popcaastra.h"
#include "../ui_popcaastra.h"
#include "userinfo.h"
#include "channelinfo.h"
#include "aastrasipnotify.h"

// TODO: make dest_nb_col a config option
#define DEST_NB_COL 6
#define SPECIAL_ME "user:special:me"

/*! \brief Constructor
 *
 * In this example, we just set a simple layout for
 * the widget and add a label displaying "Hello world !" in it.
 */
PopcAastra::PopcAastra(QWidget *parent)
    : XLet(parent), ui(new Ui::PopcAastra)
{
    m_fullname = "";
    m_sipuser = "";
    ui->setupUi(this);
    setTitle(tr("POPC Aastra operator"));
    // Signals / slots
    connect(b_engine, SIGNAL(updateUserConfig(const QString &)),
            this, SLOT(updateUserConfig(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
            this, SLOT(updatePhoneConfig(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneStatus(const QString &)),
            this, SLOT(updatePhoneStatus(const QString &)));
    connect(b_engine, SIGNAL(updateAgentConfig(const QString &)),
            this, SLOT(updateAgentConfig(const QString &)));
    connect(b_engine, SIGNAL(updateAgentStatus(const QString &)),
            this, SLOT(updateAgentStatus(const QString &)));
    connect(ui->destinationGrid, SIGNAL(cellDoubleClicked(int, int)),
                this, SLOT(destinationClicked(int, int)));
    connect(b_engine, SIGNAL(updateUserStatus(const QString &)),
                this, SLOT(refreshDestination(const QString &)));
    connect(b_engine, SIGNAL(updateChannelStatus(const QString &)),
                this, SLOT(refreshLines(const QString &)));
    connect(ui->btn_vol_up, SIGNAL(clicked()), this, SLOT(volUp()));
    connect(ui->btn_vol_down, SIGNAL(clicked()), this, SLOT(volDown()));
}

void PopcAastra::updateAgentConfig(const QString &agent)
{

}


void PopcAastra::updatePhoneStatus(const QString & phone)
{
    qDebug() << "updatePhoneStatus";
}

void PopcAastra::updatePhoneConfig(const QString & phone)
{
    qDebug() << "updatePhoneConfig";
}

void PopcAastra::updateUserConfig(const QString & user)
{
    qDebug() << "updateUserConfig";
}

void PopcAastra::volUp()
{
    emit ipbxCommand(getAastraKeyNotify(VOL_UP, SPECIAL_ME));
}

void PopcAastra::volDown()
{
    emit ipbxCommand(getAastraKeyNotify(VOL_DOWN, SPECIAL_ME));
}

void PopcAastra::destinationClicked(int col, int row)
{
    qDebug() << "Cell clicked " << col << "x" << row;
}

void PopcAastra::refreshLines(const QString &chan)
{
    if (m_fullname == "") {
        m_fullname = b_engine->getXivoClientUser()->fullname();
    }
    QHash<QString, ChannelInfo*> channels = b_engine->channels();
    ChannelInfo* chaninfo = channels.value(chan);
    if (m_sipuser == "" && chaninfo->peerdisplay() == m_fullname) {
        QStringList parts = chaninfo->xchannel().split("/");
        for (int i = 0; i < parts.size(); ++i) {
            qDebug() << QString("%1 is %2").arg(i).arg(parts.at(i));
        }
        m_sipuser = QString("SIP/%1").arg(parts.at(2).split("-").at(0));
    }

    if (m_sipuser != "" && !(chaninfo->channel().startsWith(m_sipuser, Qt::CaseInsensitive))) {
        qDebug() << "Not my channel, updating...";
        qDebug() << chaninfo->toString();
        if (m_incoming.contains(chaninfo->xchannel())) {
            // update
            m_incoming[chaninfo->xchannel()] = chaninfo;
            if (chaninfo->commstatus() == "linked-called") {
                qDebug() << "Call complete removing";
                m_incoming.erase(m_incoming.find(chaninfo->xchannel()));
            }
        } else {
            // Add
            m_incoming[chaninfo->xchannel()] = chaninfo;
        }
    }
    debugIncomingCalls();
}

void PopcAastra::debugIncomingCalls()
{
    qDebug() << "Incoming calls: " << m_incoming.size();
    QHash<QString, ChannelInfo*>::iterator i = m_incoming.begin();
    while (i != m_incoming.end()) {
        //if (! b_engine->channels().contains(i.key())) {
        //    //qDebug() << "Removing this channel";
        //    m_incoming.erase(i++);
        //    continue;
        //}
        ChannelInfo* chaninfo = i.value();
        //qDebug() << chaninfo->toString();
        qDebug() << "==================================================";
        ++i;
    }
}

void PopcAastra::debugChannels() const
{
    UserInfo* me = b_engine->getXivoClientUser();
    QString myName = me->fullname();
    qDebug() << "Name: " << myName;
    int j = 0;
    QHash<QString, ChannelInfo *>::const_iterator i = b_engine->channels().constBegin();
    while (i !=  b_engine->channels().constEnd()) {
        qDebug() << "Key:" << i.key();
        ChannelInfo* chan = i.value();
        qDebug() << "Direction: "   << chan->direction();
        qDebug() << "line number "  << chan->linenumber();
        qDebug() << "comm status "  << chan->commstatus();
        qDebug() << "Peer display " << chan->peerdisplay();
        qDebug() << "xChannel "     << chan->xchannel();
        qDebug() << "talking to "   << chan->talkingto_id();
        qDebug() << "talking to kind " << chan->talkingto_kind();
        ++i;
    }
}

void PopcAastra::refreshDestination(const QString &user)
{
    QHash<QString, UserInfo*> users = b_engine->users();
    int nb_users = users.size();
    if (nb_users == 0) return;
    int cols = DEST_NB_COL > nb_users ? nb_users : DEST_NB_COL;
    int rows = nb_users / cols;
    if (nb_users % cols) rows++;
    QTableWidget* grid = ui->destinationGrid;
    grid->setColumnCount(cols);
    grid->setRowCount(rows);
    int gridWidth = grid->width();
    int cellWidth = gridWidth / cols;
    for (int i = 0; i < cols; ++i)
        grid->setColumnWidth(i, cellWidth);
    grid->clear();
    QHash<QString, UserInfo*>::const_iterator i = users.constBegin();
    int j = 0;
    while (i != users.constEnd()) {
        grid->setItem(j / cols, j % cols, new QTableWidgetItem(
            QString("%1: %2").arg(j < 26 ? (char)('a' + j) : ' ').arg(i.value()->fullname())));
        ++i;
        ++j;
    }
}

/*! \brief destructor
 */
PopcAastra::~PopcAastra()
{
    qDebug() << Q_FUNC_INFO;
    delete ui;
}

