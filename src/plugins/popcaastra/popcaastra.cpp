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

#include "popcaastra.h"

#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "aastrasipnotify.h"
#include "channelinfo.h"
#include "completionedit.h"
#include "currentcallwidget.h"
#include "holdedwidget.h"
#include "incomingwidget.h"
#include "parkedwidget.h"
#include "popcaastra.h"
#include "transferedwidget.h"
#include "xivoconsts.h"
#include "phonenumber.h"

class QLabel;
class QTableWidget;

#include "channelinfo.h"
#include "phoneinfo.h"
#include "transferedwidget.h"
#include "userinfo.h"

#define MAX_LINES 4

PopcAastra::PopcAastra(QWidget *parent)
    : XLet(parent), m_current_call(0), m_layout(0), m_top_widget(0),
      m_timerid(0), m_btn_hangup(0), m_btn_nav_right(0),
      m_btn_vol_down(0), m_btn_vol_up(0), m_targets(0), m_contact_completer(0)
{
    setTitle(tr("POPC Aastra operator"));

    m_layout = new QVBoxLayout();
    m_top_widget = new QHBoxLayout();

    setLayout(m_layout);
    m_layout->addLayout(m_top_widget);
    m_layout->setAlignment(Qt::AlignTop);

    m_btn_hangup = new QPushButton("Hangup", this);
    m_btn_nav_right = new QPushButton("-->", this);
    m_btn_vol_down = new QPushButton("v", this);
    m_btn_vol_up = new QPushButton("^", this);
    m_targets = new FilteredLineEdit(this);

    m_top_widget->addWidget(m_btn_hangup);
    m_top_widget->addWidget(m_btn_nav_right);
    m_top_widget->addWidget(m_btn_vol_down);
    m_top_widget->addWidget(m_btn_vol_up);
    m_top_widget->addWidget(m_targets);
    m_top_widget->setSizeConstraint(QLayout::SetMinimumSize);

    fillCompleter();
    startTimer(1000);

    // Signals / slots
    connect(b_engine, SIGNAL(updatePhoneStatus(const QString &)),
            this, SLOT(updatePhoneStatus(const QString &)));
    connect(b_engine, SIGNAL(updateChannelStatus(const QString &)),
            this, SLOT(updateChannelStatus(const QString &)));
    connect(b_engine, SIGNAL(broadcastNumberSelection(const QStringList &)),
            this, SLOT(receiveNumberList(const QStringList &)));
    connect(b_engine, SIGNAL(pasteToXlets(const QString &)),
            this, SLOT(receiveNumber(const QString &)));
    connect(b_engine, SIGNAL(updateUserConfig(const QString &)),
            this, SLOT(updateUserConfig(const QString &)));
    connect(b_engine, SIGNAL(updateMeetmesConfig(const QString &)),
            this, SLOT(updateMeetmesConfig(const QString &)));

    connect(m_btn_vol_up, SIGNAL(clicked()), this, SLOT(volUp()));
    connect(m_btn_vol_down, SIGNAL(clicked()), this, SLOT(volDown()));
    connect(m_btn_nav_right, SIGNAL(clicked()), this, SLOT(navRight()));
    connect(m_btn_hangup, SIGNAL(clicked()), this, SLOT(hangup()));
    connect(m_targets, SIGNAL(textChanged(const QString &)),
            this, SLOT(targetChanged(const QString &)));
}

void PopcAastra::dial(const QString & number)
{
    QStringList commands;
    aastra_notify::AppendNumber(number, commands);
    commands.append(GetKeyUri(aastra_notify::NAV_RIGHT));
    emit ipbxCommand(aastra_notify::Build(commands, SPECIAL_ME));
}

void PopcAastra::updateUserConfig(const QString & userxid)
{
    static QString pattern = "%0 <%1>";
    if (const UserInfo * u = b_engine->user(userxid)) {
        foreach (const QString & phonexid, u->phonelist()) {
            const PhoneInfo * p = b_engine->phone(phonexid);
            if (p && ! p->number().isEmpty()) {
                QString entry = pattern.arg(u->fullname()).arg(p->number());
                m_contact_completer->insertItem(entry);
            }
        }
    }
}

void PopcAastra::updateMeetmesConfig(const QString & mxid)
{
    static QString prefix = "Conf";
    static QString pattern = "%0: %1 <%2>"; // "Conf: My meetme <800>"
    if (const MeetmeInfo * m = b_engine->meetme(mxid)) {
        QString entry = pattern.arg(prefix).arg(m->name()).arg(m->number());
        m_contact_completer->insertItem(entry);
    }
}

void PopcAastra::updateChannelStatus(const QString & cxid)
{
    bool my_channel = (m_ui && m_ui->hasChannelId(cxid));
    const ChannelInfo * c = b_engine->channel(cxid);
    if (my_channel && c && c->commstatus() == "linked-called") {
        if (m_current_call && m_current_call->channelid() == cxid) {
            m_current_call->update();
        } else {
            if (m_current_call) {
                delete m_current_call;
                m_current_call = 0;
            }
            QString dev = c->talkingto_id().split("-").value(0);
            if (const PhoneInfo * p = phone::findByIdentity(dev)) {
                m_current_call = new CurrentCallWidget(
                    p->xid(), cxid, nextLine(), this);
                m_current_call->update();
                m_layout->addWidget(m_current_call);
                return;
            }
        }
    } else if (my_channel && c && c->commstatus() != "linked-called"
               && c->direction() == "in") {
        if (m_incomingcalls.contains(cxid)) {
            m_incomingcalls[cxid]->update();
        } else {
            QString identity = c->id().split("-").value(0);
            if (const PhoneInfo * p = phone::findByIdentity(identity)) {
                IncomingWidget * w = new IncomingWidget(
                    nextLine(), cxid, p->xid(), this);
                w->update();
                m_incomingcalls[cxid] = w;
                m_layout->addWidget(w);
            }
        }
    }
}

void PopcAastra::removeDefunctWidgets()
{
    if (m_current_call && m_current_call->toRemove()) {
        delete m_current_call;
        m_current_call = 0;
    }

    if (m_pending_to_remove.size()) {
        foreach (const QString & key, m_pending_to_remove) {
            if (m_pendingcalls.contains(key)) {
                delete m_pendingcalls[key];
                m_pendingcalls.remove(key);
            }
        }
        m_pending_to_remove.clear();
    }

    if (m_incoming_to_remove.size()) {
        foreach (const QString & key, m_incoming_to_remove) {
            if (m_incoming_to_remove.size()) {
                delete m_incomingcalls[key];
                m_incomingcalls.remove(key);
            }
        }
        m_incoming_to_remove.clear();
    }
}

void PopcAastra::removeCompletedPendings()
{
    if (0 == m_pendingcalls.size())
        return;

    QStringList to_remove;
    foreach (const QString & device_key, m_pendingcalls.keys()) {
        if (m_pendingcalls[device_key]->toRemove()) {
            to_remove << device_key;
        }
    }

    foreach (const QString & device_key, to_remove) {
        removePendingCall(device_key);
    }
}

void PopcAastra::removeIncomingCall(const QString & key)
{
    if (m_incomingcalls.contains(key)) {
        delete m_incomingcalls[key];
        m_incomingcalls.remove(key);
    }
}

void PopcAastra::removePendingCall(const QString & key)
{
    if (m_pendingcalls.contains(key)) {
        delete m_pendingcalls[key];
        m_pendingcalls.remove(key);
    }
}

int PopcAastra::nextLine() const {
    if (m_incomingcalls.size() == 0) {
        return 1;
    }
    for (int i = 0; i < MAX_LINES; i++) {
        bool lineFree = true;
        foreach (const QString & key, m_incomingcalls.keys()) {
            if (m_incomingcalls[key]->line() == i + 1) {
                lineFree = false;
            }
        }
        if (lineFree) {
            return i + 1;
        }
    }
    return -1;
}

void PopcAastra::updatePhoneStatus(const QString & xphoneid)
{
    /* 0 = Available
       1 = Talking
       4 = Not available
       8 = Ringing
       9 = (On line OR calling) AND ringing
       16 = On hold */
    removeDefunctWidgets();
    removeCompletedPendings();
    if (const PhoneInfo * phone = b_engine->phone(xphoneid)) {
        foreach (const QString & key, phone->xchannels()) {
            if (m_pendingcalls.contains(key)
                && m_pendingcalls[key]->phonexid() == xphoneid) {
                m_pendingcalls[key]->update();
            }
            if (m_incomingcalls.contains(key)
                && m_incomingcalls[key]->phonexid() == xphoneid) {
                m_incomingcalls[key]->update();
            }
        }
    }
}

void PopcAastra::timerEvent(QTimerEvent *)
{
    if (m_pendingcalls.size()) {
        removeCompletedPendings();
    }

    if (m_incomingcalls.size()) {
        removeDefunctWidgets();
    }

    if (m_current_call) {
        m_current_call->update();
    }

    foreach (const QString & key, m_incomingcalls.keys())
        m_incomingcalls[key]->update();
    foreach (const QString & key, m_pendingcalls.keys())
        m_pendingcalls[key]->update();
}

void PopcAastra::conf()
{
    QString mxid = promptMeetme();
    if (const MeetmeInfo * m = b_engine->meetme(mxid)) {
        QStringList commands = QStringList()
            << aastra_notify::GetKeyUri(aastra_notify::XFER);
        aastra_notify::AppendNumber(m->number(), commands);
        commands.append(aastra_notify::GetKeyUri(aastra_notify::XFER));
        emit ipbxCommand(aastra_notify::Build(commands, SPECIAL_ME));
    }
}

void PopcAastra::hangupLine(int line)
{
    QStringList commands;
    // commands.append(aastra_notify::GetKeyUri(aastra_notify::LINE, line));
    commands.append(aastra_notify::GetKeyUri(aastra_notify::GOODBYE));
    emit ipbxCommand(aastra_notify::Build(commands, SPECIAL_ME));
}

void PopcAastra::holdLine(const QString & device_identity, int line)
{
    QStringList commands;
    commands.append(aastra_notify::GetKeyUri(aastra_notify::LINE, line));
    commands.append(aastra_notify::GetKeyUri(aastra_notify::HOLD));
    emit ipbxCommand(aastra_notify::Build(commands, SPECIAL_ME));
    if (const PhoneInfo * p = phone::findByIdentity(device_identity)) {
        trackHolded(p->xid(), line);
    }
}

void PopcAastra::trackHolded(const QString & phonexid, int line)
{
    if (PendingWidget * w = new HoldedWidget(phonexid, line, this)) {
        if (const PhoneInfo * p = b_engine->phone(phonexid)) {
            m_pendingcalls[p->identity()] = w;
            m_layout->addWidget(w);
            w->update();
        } else {
            delete w;
        }
    }
}

void PopcAastra::transfer()
{
    QStringList commands = QStringList()
        << aastra_notify::GetKeyUri(aastra_notify::XFER);
    aastra_notify::AppendNumber(m_selected_number, commands);
    commands.append(aastra_notify::GetKeyUri(aastra_notify::XFER));
    emit ipbxCommand(aastra_notify::Build(commands, SPECIAL_ME));
    trackTransfer(m_current_call->phonexid());
}

void PopcAastra::trackTransfer(const QString & pxid)
{
    if (const PhoneInfo * p = b_engine->phone(pxid)) {
        if (PendingWidget * w = new TransferedWidget(
                pxid, m_selected_number, this)) {
            w->update();
            m_pendingcalls[p->identity()] = w;
            m_layout->addWidget(w);
        }
    } else {
        qDebug() << "Cannot track this phone";
    }
}

void PopcAastra::attendedTransfer()
{
    QStringList commands;
    commands.append(aastra_notify::GetKeyUri(aastra_notify::XFER));
    aastra_notify::AppendNumber(m_selected_number, commands);
    commands.append(aastra_notify::GetKeyUri(aastra_notify::NAV_RIGHT));
    emit ipbxCommand(aastra_notify::Build(commands, SPECIAL_ME));
}

void PopcAastra::doIntercept(const QString & exten)
{
    QStringList commands;
    commands.append(aastra_notify::GetKeyUri(aastra_notify::KEYPAD_STAR));
    commands.append(aastra_notify::GetKeyUri(aastra_notify::KEYPAD, 8));
    aastra_notify::AppendNumber(exten, commands);
    commands.append(aastra_notify::GetKeyUri(aastra_notify::NAV_RIGHT));
    emit ipbxCommand(aastra_notify::Build(commands, SPECIAL_ME));
}

void PopcAastra::remove_pending(unsigned int id)
{
    foreach (const QString & key, m_pendingcalls.keys()) {
        if (m_pendingcalls[key]->id() == id) {
            m_pending_to_remove.append(key);
        }
    }
}

void PopcAastra::remove_incoming(int line)
{
    foreach (const QString & key, m_incomingcalls.keys()) {
        if (m_incomingcalls[key]->line() == line) {
            m_incoming_to_remove.append(key);
        }
    }
}

void PopcAastra::selectLine(int line)
{
    emit ipbxCommand(aastra_notify::GetKey(
                         aastra_notify::LINE, SPECIAL_ME, line));
}

void PopcAastra::park()
{
    QString pxid = promptParking();
    if (const ParkingInfo * parking = b_engine->parkinglot(pxid)) {
        QStringList commands = QStringList()
            << aastra_notify::GetKeyUri(aastra_notify::XFER);
        aastra_notify::AppendNumber(parking->number(), commands);
        commands.append(aastra_notify::GetKeyUri(aastra_notify::XFER));
        emit ipbxCommand(aastra_notify::Build(commands, SPECIAL_ME));
        trackParked(pxid, m_current_call->phonexid());
    }
}

QString PopcAastra::promptParking() const
{
    QMap<QString, QPushButton *> parking_map;
    QMessageBox box;
    foreach (const QString & pxid, b_engine->iterover("parkinglots").keys()) {
        if (const ParkingInfo * p = b_engine->parkinglot(pxid)) {
            parking_map[p->xid()] = box.addButton(QString("%0 - %1")
                                                  .arg(p->number()).arg(p->name()),
                                                  QMessageBox::ActionRole);
            if (parking_map.size() == 1) {
                box.setDefaultButton(parking_map[p->xid()]);
            }
        }
    }

    if (parking_map.size() > 0) {
        box.setText(tr("Choose the parking to park the call to"));
        box.addButton(QMessageBox::Cancel);
        box.exec();
        foreach (const QString & pxid, b_engine->iterover("parkinglots").keys()) {
            const ParkingInfo * p = b_engine->parkinglot(pxid);
            if (p && box.clickedButton() == parking_map[p->xid()]) {
                return pxid;
            }
        }
    } else {
        qDebug() << Q_FUNC_INFO << "No parking available";
    }
    qDebug() << Q_FUNC_INFO << "No parking selected";
    return QString();
}

QString PopcAastra::promptMeetme() const
{
    QMap<QString, QPushButton *> meetme_map;
    QMessageBox box;
    foreach (const QString & mxid, b_engine->iterover("meetmes").keys()) {
        const MeetmeInfo * m = b_engine->meetme(mxid);
        if (! m) continue;
        meetme_map[m->xid()] = box.addButton(QString("%0 - %1")
                                             .arg(m->number()).arg(m->name()),
                                             QMessageBox::ActionRole);
        if (meetme_map.size() == 1) {
            box.setDefaultButton(meetme_map[m->xid()]);
        }
    }

    if (meetme_map.size() > 0) {
        box.setText(tr("Choose the conference room to transfer this call to"));
        box.addButton(QMessageBox::Cancel);
        box.exec();
        foreach (const QString & mxid, b_engine->iterover("meetmes").keys()) {
            const MeetmeInfo * m = b_engine->meetme(mxid);
            if (m && box.clickedButton() == meetme_map[m->xid()]) {
                return m->xid();
            }
        }
    } else {
        qDebug() << Q_FUNC_INFO << "No meetme available";
    }
    qDebug() << Q_FUNC_INFO << "No meetme selected";
    return QString();
}

void PopcAastra::trackParked(const QString & parkingxid,
                             const QString & phonexid)
{
    if (PendingWidget * w = new ParkedWidget(phonexid, parkingxid, this)) {
        w->update();
        m_pendingcalls[phonexid] = w;
        m_layout->addWidget(w);
    }
}

void PopcAastra::volUp()
{
    emit ipbxCommand(aastra_notify::GetKey(aastra_notify::VOL_UP, SPECIAL_ME));
}

void PopcAastra::volDown()
{
    emit ipbxCommand(aastra_notify::GetKey(aastra_notify::VOL_DOWN, SPECIAL_ME));
}

void PopcAastra::navRight()
{
    emit ipbxCommand(aastra_notify::GetKey(
                         aastra_notify::NAV_RIGHT, SPECIAL_ME));
}

void PopcAastra::hangup()
{
    emit ipbxCommand(aastra_notify::GetKey(aastra_notify::GOODBYE, SPECIAL_ME));
}

void PopcAastra::hold()
{
    emit ipbxCommand(GetKey(aastra_notify::HOLD, SPECIAL_ME));
    trackHolded(m_current_call->phonexid(), m_current_call->line());
}

void PopcAastra::remove_current_call()
{
    if (m_current_call) {
        delete m_current_call;
        m_current_call = 0;
    }
}

void PopcAastra::prgkey1()
{
    emit ipbxCommand(aastra_notify::GetKey(
                         aastra_notify::PRG_KEY, SPECIAL_ME, 1));
}

void PopcAastra::receiveNumberList(const QStringList & numbers)
{
    if (numbers.isEmpty()) {
        return;
    } else if (numbers.size() == 1) {
        m_selected_number = numbers.at(0);
    } else {
        // FIXME: take multiple numbers into account
        m_selected_number = numbers.at(0);
    }
    m_targets->setText(m_selected_number);
}

void PopcAastra::receiveNumber(const QString & number)
{
    if (number.isEmpty()) return;
    m_targets->setText(number);
}

void PopcAastra::targetChanged(const QString & text)
{
    // qDebug() << Q_FUNC_INFO << text;
    m_selected_number = PhoneNumber::extract(text);
}

void PopcAastra::fillCompleter()
{
    if ((m_contact_completer = new FilteredCompleter(this)) != 0) {
        m_contact_completer->setCaseSensitivity(Qt::CaseInsensitive);
        m_contact_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
        m_targets->setCompleter(m_contact_completer);
    }
}
