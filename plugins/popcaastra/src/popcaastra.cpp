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
#include "phoneinfo.h"
#include "transferedwidget.h"
#include "userinfo.h"

#define MAX_LINES 4

PopcAastra::PopcAastra(QWidget *parent)
    : XLet(parent), m_current_call(0)
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

    m_timerid = 0;
    m_deltasec = 1;
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
    connect(b_engine, SIGNAL(updateUserStatus(const QString &)),
            this, SLOT(updateUserStatus(const QString &)));
    connect(b_engine, SIGNAL(updateMeetmesConfig(const QString &)),
            this, SLOT(updateMeetmesConfig(const QString &)));

    connect(m_btn_vol_up, SIGNAL(clicked()), this, SLOT(volUp()));
    connect(m_btn_vol_down, SIGNAL(clicked()), this, SLOT(volDown()));
    connect(m_btn_nav_right, SIGNAL(clicked()), this, SLOT(navRight()));
    connect(m_btn_hangup, SIGNAL(clicked()), this, SLOT(hangup()));
    connect(m_targets, SIGNAL(textChanged(const QString &)),
            this, SLOT(targetChanged(const QString &)));
}

/*! \brief Update status for incoming calls widget list
 *
 *  Goes thru each incoming calls in the GUI and update the status of each
 *  calls
 */
void PopcAastra::updateDisplay()
{
    foreach (const QString & key, m_incomingcalls.keys()) {
        m_incomingcalls[key]->update();
    }
    foreach (const QString & key, m_pendingcalls.keys()) {
        m_pendingcalls[key]->update();
    }
}

/*! \brief Dial a number using Aastra SIP notify */
void PopcAastra::dial(const QString & number)
{
    QStringList commands;
    for (int i = 0; i < number.size(); ++i) {
        const QChar & c = number[i];
        if (c.isDigit()) {
            commands.append(getKeyUri(KEYPAD, c.digitValue()));
        }
    }
    commands.append(getKeyUri(NAV_RIGHT));
    emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
}

/*! \brief Add users to transfer targets
 *  \param xUId The user's Xivo id
 */
void PopcAastra::updateUserStatus(const QString & xUId)
{
    static QString pattern = "%0 <%1>";
    const UserInfo * u = b_engine->user(xUId);
    if (u) {
        foreach (const QString & phonexid, u->phonelist()) {
            const PhoneInfo * p = b_engine->phone(phonexid);
            if (p && ! p->number().isEmpty()) {
                QString entry = pattern.arg(u->fullname()).arg(p->number());
                m_contact_completer->insertItem(entry);
            }
        }
    }
}

/*! \brief Update the target list with the available conf rooms */
void PopcAastra::updateMeetmesConfig(const QString & mxid)
{
    static QString prefix = "Conf";
    static QString pattern = "%0: %1 <%2>"; // "Conf: My meetme <800>"
    const MeetmeInfo * m = b_engine->meetme(mxid);
    if (m) {
        QString entry = pattern.arg(prefix).arg(m->name()).arg(m->number());
        m_contact_completer->insertItem(entry);
    }
}

/*! \brief receives channel status updates from the base engine
 *
 *  When ever a channel status changes, the base engine sends a signal that
 *  we catch to update our incoming and transfered call widgets
 *
 *  \param cxid The pbx/channel id of the updated channel
*/
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
            const PhoneInfo * p = phone::findByIdentity(dev);
            if (p) {
                m_current_call = new CurrentCallWidget(
                    p->xid(), cxid, nextLine(), this);
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
            const PhoneInfo * p = phone::findByIdentity(identity);
            if (p) {
                IncomingWidget * w = new IncomingWidget(
                    nextLine(), cxid, p->xid(), this);
                m_incomingcalls[cxid] = w;
                m_layout->addWidget(w);
            }
        }
    }
}

/*! \brief Removes defunct call widgets for defunct channels
 *
 *  Check the tracked widgets for defunct channels and remove defunct ones
 *  from the incoming list
 */
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

    if (m_incomingcalls.size() == 0 && m_pendingcalls.size() == 0) return;

    foreach (const QString & cxid, m_incomingcalls.keys()) {
        const ChannelInfo * c = b_engine->channel(cxid);
        if (!c || (c && c->isparked())) {
            removeIncomingCall(cxid);
        }
    }

    // Remove tracked transfers
}

/*! \brief Remove completed blind transfers from the widget list
 *
 * Remove transfered calls that are not in calling state anymore
 */
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

/*! \brief Removes a widget from the incoming call list
 *
 *  Remove a widget from the incoming call list if it's part of the list
 *  \param key The widget's key in the list
 */
void PopcAastra::removeIncomingCall(const QString & key)
{
    if (m_incomingcalls.contains(key)) {
        delete m_incomingcalls[key];
        m_incomingcalls.remove(key);
    }
}

/*! \brief Removes a widget from the pending call list
 * \param key
 */
void PopcAastra::removePendingCall(const QString & key)
{
    if (m_pendingcalls.contains(key)) {
        delete m_pendingcalls[key];
        m_pendingcalls.remove(key);
    }
}

/*! \brief finds the first line available to place this channel
 *
 *  The goal is to know/guess on which line a call is to avoid
 *  mistakes when doing operations to a call since we are dealing
 *  with lines on the phone device when using aastra xml api
 *  Return -1 when all the lines are taken
 */
int PopcAastra::nextLine() const {
    if (m_incomingcalls.size() == 0) {
        return 1;
    }
    for (int i = 0; i < MAX_LINES; i++) {
        bool lineFree = true;
        foreach (QString channel, m_incomingcalls.keys()) {
            if (m_incomingcalls[channel]->line() == i + 1) lineFree = false;
        }
        if (lineFree) return i + 1;
    }
    return -1;
}

/*! \brief parse new phone status
 */
void PopcAastra::updatePhoneStatus(const QString & xphoneid)
{
    /* 0 = Available
       1 = Talking
       4 = Not available
       8 = Ringing
       9 = (On line OR calling) AND ringing
       16 = On hold */
    // qDebug() << Q_FUNC_INFO << xphoneid;
    removeDefunctWidgets();
    removeCompletedPendings();
    const PhoneInfo * phone = b_engine->phone(xphoneid);
    if (phone == NULL) return;

    foreach (QString xchannel, phone->xchannels()) {
        if (m_pendingcalls.contains(xchannel)) {
            PendingWidget * w = m_pendingcalls[xchannel];
            w->update();
        }
        if (m_incomingcalls.contains(xchannel)) {
            IncomingWidget * w = m_incomingcalls[xchannel];
            w->update();
        }
    }
}

void PopcAastra::timerEvent(QTimerEvent * /* event */)
{
    if (m_current_call) {
        m_current_call->update();
    }

    foreach (QString key, m_incomingcalls.keys())
        m_incomingcalls[key]->update();
    foreach (QString key, m_pendingcalls.keys())
        m_pendingcalls[key]->update();
    if (m_pendingcalls.size() || m_incomingcalls.size()) {
        removeDefunctWidgets();
        removeCompletedPendings();
    }
}

void PopcAastra::conf()
{
    QString mxid = promptMeetme();
    const MeetmeInfo * m = b_engine->meetme(mxid);
    if (m) {
        QStringList commands = QStringList()
            << getKeyUri(XFER);
        const QString & num = m->number();
        for (int i = 0; i < num.size(); ++i) {
            const QChar & c = num[i];
            if (c.isDigit()) {
                commands.append(getKeyUri(KEYPAD, c.digitValue()));
            }
        }
        commands.append(getKeyUri(XFER));
        emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
    }
}

/*! \brief Hang up a line on our phone
 *
 *  Hang up a given line on our phone using Aastra sipnotify, it has the same
 *  result as pressing the hang up button on the phone
 *
 *  \param line the line number to hangup
 */
void PopcAastra::hangUpLine(int /* line */)
{
    QList<QString> commands;
    commands.append(getKeyUri(GOODBYE));
    emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
}

/*! \brief Put this line on hold
 *
 *  Press the hold button on the phone
 *
 * \param device_identity the phone's protocol/id needed for tracking
 * \param line The phone's line to hangup
 */
void PopcAastra::holdLine(const QString & device_identity, int line)
{
    QList<QString> commands;
    commands.append(getKeyUri(LINE, line));
    commands.append(getKeyUri(HOLD));
    emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
    const PhoneInfo * p = phone::findByIdentity(device_identity);
    if (p) {
        trackHolded(p->xid(), line);
    }
}

/*! \brief Creates a widget for a call on hold and add it to the pending calls
 * \param phonexid The phone's XiVO id
 * \param line The line number on the phone
 */
void PopcAastra::trackHolded(const QString & phonexid, int line)
{
    PendingWidget * w = new HoldedWidget(phonexid, line, this);
    if (w) {
        const PhoneInfo * p = b_engine->phone(phonexid);
        if (p) {
            m_pendingcalls[p->identity()] = w;
            m_layout->addWidget(w);
        } else {
            delete w;
        }
    }
}

void PopcAastra::transfer()
{
    QStringList commands = QStringList()
        << getKeyUri(XFER);
    for (int i = 0; i < m_selected_number.size(); ++i) {
        const QChar & c = m_selected_number[i];
        if (c.isDigit()) {
            commands.append(getKeyUri(KEYPAD, c.digitValue()));
        }
    }
    commands.append(getKeyUri(XFER));
    emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
    trackTransfer(m_current_call->phonexid());
}

/*! \brief Track a call after a blind transfer
 *
 * Calls are tracked after blind transfer to allow the interception of
 * unanswered calls. The key of the transfered call hash is the target number
 * of the transfer.
 * \param pxid The transfered device's XiVO id
 */
void PopcAastra::trackTransfer(const QString & pxid)
{
    const PhoneInfo * p = b_engine->phone(pxid);
    if (p) {
        PendingWidget * w = new TransferedWidget(
            pxid, m_selected_number, this);
        m_pendingcalls[p->identity()] = w;
        m_layout->addWidget(w);
    } else {
        qDebug() << "Cannot track this phone";
    }
}

/*! \brief attended transfer to the line in the number/name field */
void PopcAastra::attendedTransfer()
{
    QList<QString> commands;
    commands.append(getKeyUri(XFER));
    QString number = m_selected_number;
    for (int i = 0; i < number.size(); ++i) {
        const QChar c = number[i];
        if (c.isDigit())
            commands.append(getKeyUri(KEYPAD, c.digitValue()));
    }
    commands.append(getKeyUri(NAV_RIGHT));
    emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
}

/*! \brief Intercept a call ringing at the given number
 *
 *  Sends a *8 exten on the phone to intercept a call ringing on a given
 *  exten
 *
 *  \param exten The exten number to intercept
 */
void PopcAastra::doIntercept(const QString & exten)
{
    QList<QString> commands;
    commands.append(getKeyUri(KEYPAD_STAR));
    commands.append(getKeyUri(KEYPAD, 8));
    for (int i = 0; i < exten.size(); ++i) {
        const QChar c = exten[i];
        if (c.isDigit())
            commands.append(getKeyUri(KEYPAD, c.digitValue()));
    }
    commands.append(getKeyUri(NAV_RIGHT));
    emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
}

/*! \brief Add a pending call to the removal list */
void PopcAastra::remove_pending(unsigned int id)
{
    foreach (const QString & key, m_pendingcalls.keys()) {
        if (m_pendingcalls.contains(key)
            && m_pendingcalls[key]->id() == id) {
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

/*! \brief select the line */
void PopcAastra::selectLine(int line)
{
    emit ipbxCommand(getAastraKeyNotify(LINE, SPECIAL_ME, line));
}

void PopcAastra::park()
{
    QString pxid = promptParking();
    const ParkingInfo * parking = b_engine->parkinglot(pxid);
    if (parking) {
        const QString & number = parking->number();
        QStringList commands = QStringList()
            << getKeyUri(XFER);
        for (int i = 0; i < number.size(); ++i) {
            const QChar c = number[i];
            if (c.isDigit()) {
                commands.append(getKeyUri(KEYPAD, c.digitValue()));
            }
        }
        commands.append(getKeyUri(XFER));
        emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
        trackParked(pxid, m_current_call->phonexid());
    }
}

/*! \brief Prompt the user for a parking
 * \return the parkinglot XiVO id */
QString PopcAastra::promptParking() const
{
    QMap<QString, QPushButton *> parking_map;
    QMessageBox box;
    foreach (const QString & pxid, b_engine->iterover("parkinglots").keys()) {
        const ParkingInfo * p = b_engine->parkinglot(pxid);
        if (! p) continue;
        parking_map[p->xid()] = box.addButton(QString("%0 - %1")
                                              .arg(p->number()).arg(p->name()),
                                              QMessageBox::ActionRole);
        if (parking_map.size() == 1) {
            box.setDefaultButton(parking_map[p->xid()]);
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
    qDebug() << Q_FUNC_INFO << parkingxid << phonexid;
    PendingWidget * w = new ParkedWidget(phonexid, parkingxid, this);
    if (w) {
        m_pendingcalls[phonexid] = w;
        m_layout->addWidget(w);
    }
}

/*! \brief turns the volume up */
void PopcAastra::volUp()
{
    emit ipbxCommand(getAastraKeyNotify(VOL_UP, SPECIAL_ME));
}

/*! \brief turns the volume down */
void PopcAastra::volDown()
{
    emit ipbxCommand(getAastraKeyNotify(VOL_DOWN, SPECIAL_ME));
}

/*! \brief press the navigation right button of the device */
void PopcAastra::navRight()
{
    emit ipbxCommand(getAastraKeyNotify(NAV_RIGHT, SPECIAL_ME));
}

/*! \brief hang up the active line */
void PopcAastra::hangup()
{
    emit ipbxCommand(getAastraKeyNotify(GOODBYE, SPECIAL_ME));
}

void PopcAastra::hold()
{
    emit ipbxCommand(getAastraKeyNotify(HOLD, SPECIAL_ME));
    trackHolded(m_current_call->phonexid(), m_current_call->line());
}

void PopcAastra::remove_current_call()
{
    if (m_current_call) {
        delete m_current_call;
        m_current_call = 0;
    }
}

/*! \brief simulates a press on the programmable button 1 */
void PopcAastra::prgkey1()
{
    // qDebug() << Q_FUNC_INFO;
    emit ipbxCommand(getAastraKeyNotify(PRG_KEY, SPECIAL_ME, 1));
}

/*! \brief receive a list of numbers for a selected peer or contact */
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

/*! \brief Receive a number from another xlet
 *
 *  Receive this number from other xlet and set the selection field to this
 *  value.
 *  \param number The received number
 */
void PopcAastra::receiveNumber(const QString & number)
{
    if (number.isEmpty()) return;
    m_targets->setText(number);
}

/*! \brief Receive changes from the target field
 *
 *  This event is received when the user type data in the field or when the
 *  value is changed programmatically
 *
 *  \param text The new value
 */
void PopcAastra::targetChanged(const QString & text)
{
    QRegExp num_regex = QRegExp("[<]?[0-9]+[>]?");
    int end = text.lastIndexOf(num_regex);
    int start = text.indexOf(num_regex);
    if (start >= 0 && text[start] == '<') {
        start++;
    }
    if (start >= 0 && start <= end) {
        QString tmp = text.left(end + 1);
        m_selected_number = tmp.right(end - start + 1);
    }
}

/*! \brief Set the completion list for the completion targets
 *
 *   The completer should contain peers, contacts, conference and parkings
 */
void PopcAastra::fillCompleter()
{
    m_contact_completer = new FilteredCompleter(this);
    m_contact_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_contact_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

    m_targets->setCompleter(m_contact_completer);
}

/*! \brief destructor
 */
PopcAastra::~PopcAastra()
{
}

