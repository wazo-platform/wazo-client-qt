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
#include <QPushButton>
#include <QVBoxLayout>

#include "aastrasipnotify.h"
#include "completionedit.h"
#include "incomingwidget.h"
#include "popcaastra.h"
#include "transferedwidget.h"
#include "channelinfo.h"
#include "phoneinfo.h"
#include "userinfo.h"

#define MAX_LINES 4

PopcAastra::PopcAastra(QWidget *parent) : XLet(parent)
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
    foreach (const QString key, m_incomingcalls.keys()) {
        m_incomingcalls[key]->updateWidget();
    }
    foreach (const QString key, m_pendingcalls.keys()) {
        m_pendingcalls[key]->update();
    }
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
    if (m_incomingcalls.contains(cxid)) {
        m_incomingcalls[cxid]->updateWidget();
    } else if (m_pendingcalls.contains(cxid)) {
        m_pendingcalls[cxid]->update();
    } else {
        const ChannelInfo * c = b_engine->channel(cxid);
        if (c) {
            const QString & peer_identity = c->talkingto_id().split("-").value(0);
            if (m_ui && m_ui->identitylist().contains(peer_identity)) {
                IncomingWidget * w = new IncomingWidget(
                    findFirstAvailableLine(), cxid, this);
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
    
    // QStringList to_remove;

    // foreach (const QString & device_key, m_pendingcalls.keys()) {
    //     bool matched = false;
    //     foreach (const ChannelInfo * c, b_engine->channels()) {
    //         if (c->xid().contains(device_key)) {
    //             matched = true;
    //             QString called_device = c->talkingto_id().split("-").at(0);
    //             if (! m_ui->identitylist().contains(called_device) && c->direction() == "out") {
    //                 if (c->commstatus() != "calling") {
    //                     to_remove << device_key;
    //                 }
    //             }
    //         }
    //     }
    //     if (! matched) {
    //         to_remove << device_key;
    //     }
    // }

    // foreach (const QString & device_key, to_remove) {
    //     removeTransferedCall(device_key);
    // }
}

/*! \brief Removes a widget from the incoming call list
 *
 *  Remove a widget from the incoming call list if it's part of the list
 *  \param key The widget's key in the list
 */
void PopcAastra::removeIncomingCall(const QString & key)
{
    // qDebug() << Q_FUNC_INFO << key;
    if (m_incomingcalls.contains(key)) {
        delete m_incomingcalls[key];
        m_incomingcalls.remove(key);
    }
}

/*! \brief Removes a widget from the transfered call list
 * \param key
 */
void PopcAastra::removeTransferedCall(const QString & key)
{
    // qDebug() << Q_FUNC_INFO << key;
    if (m_pendingcalls.contains(key)) {
        delete m_pendingcalls[key];
        m_pendingcalls.remove(key);
    }
}

/*! \brief prints the content of m_incomingcalls */
void PopcAastra::debugIncomingCalls() const
{
    qDebug() << Q_FUNC_INFO << "Number of incoming calls " << m_incomingcalls.size();
    for (QHash<QString, IncomingWidget *>::const_iterator i = m_incomingcalls.constBegin();
        i != m_incomingcalls.constEnd(); ++i) {
        IncomingWidget * current = i.value();
        qDebug() << Q_FUNC_INFO << i.key() << current->toString();
    }
}

/*! \brief finds the first line available to place this channel
 *
 *  The goal is to know/guess on which line a call is to avoid
 *  mistakes when doing operations to a call since we are dealing
 *  with lines on the phone device when using aastra xml api
 *  Return -1 when all the lines are taken
 */
int PopcAastra::findFirstAvailableLine() const {
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
            w->updateWidget();
        }
    }
}

void PopcAastra::timerEvent(QTimerEvent * /* event */)
{
    //qDebug() << Q_FUNC_INFO;
    foreach (QString key, m_incomingcalls.keys())
        m_incomingcalls[key]->updateWidget();
    foreach (QString key, m_pendingcalls.keys())
        m_pendingcalls[key]->update();
    if (m_pendingcalls.size() || m_incomingcalls.size()) {
        removeDefunctWidgets();
        removeCompletedPendings();
    }
}

/*! \brief Sends this line in a conference room
 *
 *  Sends this call to a default conference room
 *  The transfer is done using the transfer button, not the conference button
 *
 *  \param line The phone's line to transfer to the conference room
 *  \param mxid The meetme's XiVO id
 */
void PopcAastra::confLine(int, const QString & mxid)
{
    const MeetmeInfo * m = b_engine->meetme(mxid);
    if (m) {
        QList<QString> commands;
        commands.append(getKeyUri(XFER));
        QString number = m->number();
        for (int i = 0; i < number.size(); ++i) {
            const QChar c = number[i];
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
    // qDebug() << Q_FUNC_INFO << line;
    QList<QString> commands;
    // commands.append(getKeyUri(LINE, line));
    commands.append(getKeyUri(GOODBYE));
    emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
}

/*! \brief Put this line on hold
 *
 *  Press the hold button on the phone
 *
 *  \param line The phone's line to hangup
 */
void PopcAastra::holdLine(int line)
{
    // qDebug() << Q_FUNC_INFO << line;
    QList<QString> commands;
    commands.append(getKeyUri(LINE, line));
    commands.append(getKeyUri(HOLD));
    emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
}

/*! \brief Sends a blind transfer request to the phone
 *
 * Send an aastra xml request to the phone to transfer the call on line \a line
 * to the selected number
 * \param peers_device The techno/identifier of our peer's device (SIP/abc)
 * \param line The phone's line number
 * \param transferedname The name of the transfered caller
 * \param transferednumber The number of the transfered caller
 */
void PopcAastra::blindTransfer(const QString & device_identity,
                               int line,
                               const QString & t_name,
                               const QString & t_num)
{
    qDebug() << Q_FUNC_INFO << device_identity << line << t_name << t_num;
    QList<QString> commands;
    commands.append(getKeyUri(LINE, line));
    commands.append(getKeyUri(XFER));
    QString number = m_selected_number;
    for (int i = 0; i < number.size(); ++i) {
        const QChar c = number[i];
        if (c.isDigit()) {
            commands.append(getKeyUri(KEYPAD, c.digitValue()));
        }
    }
    commands.append(getKeyUri(XFER));

    QString phonexid;
    foreach (const QString & key, b_engine->iterover("phones").keys()) {
        if (b_engine->phone(key)->identity() == device_identity) {
            phonexid = key;
            break;
        }
    }

    trackTransfer(phonexid, t_name, t_num);
    emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
}

/*! \brief Track a call after a blind transfer
 *
 * Calls are tracked after blind transfer to allow the interception of
 * unanswered calls. The key of the transfered call hash is the target number
 * of the transfer.
 * \param pxid The transfered device's XiVO id
 * \param tname The transfered person's name
 * \param tnum The transfered person's number
 */
void PopcAastra::trackTransfer(const QString & pxid,
                               const QString & tname,
                               const QString & tnum)
{
    qDebug() << Q_FUNC_INFO << pxid << tname << tnum;
    const PhoneInfo * p = b_engine->phone(pxid);
    if (p) {
        TransferedWidget * w = new TransferedWidget(pxid, tname, tnum, this);
        m_pendingcalls[p->identity()] = w;
        m_layout->addWidget(w);
        // connect(w, SIGNAL(intercept(const QString &)),
        //         this, SLOT(doIntercept(const QString &)));
    } else {
        qDebug() << "Cannot track this phone";
    }
}

/*! \brief attended transfer to the line in the number/name field */
void PopcAastra::attendedTransfer(int line)
{
    // qDebug() << Q_FUNC_INFO << line;
    QList<QString> commands;
    commands.append(getKeyUri(LINE, line));
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
    // qDebug() << Q_FUNC_INFO << exten;
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

/*! \brief select the line */
void PopcAastra::selectLine(int line)
{
    // qDebug() << Q_FUNC_INFO << line;
    emit ipbxCommand(getAastraKeyNotify(LINE, SPECIAL_ME, line));
}

/*! \brief transfer the call to a parking lot
 *  \param line to transfer
 *  \param pxid the parking's XiVO id
 *  \param device The device id of the transfered call
 */
void PopcAastra::parkcall(int line, const QString & pxid, const QString & device)
{
    foreach (const XInfo * p, b_engine->iterover("phones")) {
        const PhoneInfo * phone = static_cast<const PhoneInfo *>(p);
        if (phone->identity() == device) {
            QString userxid = (QString("%0/%1").arg(phone->ipbxid())
                               .arg(phone->iduserfeatures()));
            const UserInfo * u = b_engine->user(userxid);
            const ParkingInfo * park = b_engine->parkinglot(pxid);
            if (u && park) {
                const QString & number = park->number();
                QStringList commands = QStringList()
                    << getKeyUri(LINE, line)
                    << getKeyUri(XFER);
                for (int i = 0; i < number.size(); ++i) {
                    const QChar c = number[i];
                    if (c.isDigit()) {
                        commands.append(getKeyUri(KEYPAD, c.digitValue()));
                    }
                }
                commands.append(getKeyUri(XFER));
                emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
                // return trackTransfer(device, park->number(),
                //                      u->fullname(), phone->number());
            }
        }
    }
}

/*! \brief turns the volume up */
void PopcAastra::volUp()
{
    // qDebug() << Q_FUNC_INFO;
    emit ipbxCommand(getAastraKeyNotify(VOL_UP, SPECIAL_ME));
}

/*! \brief turns the volume down */
void PopcAastra::volDown()
{
    // qDebug() << Q_FUNC_INFO;
    emit ipbxCommand(getAastraKeyNotify(VOL_DOWN, SPECIAL_ME));
}

/*! \brief press the navigation right button of the device */
void PopcAastra::navRight()
{
    // qDebug() << Q_FUNC_INFO;
    emit ipbxCommand(getAastraKeyNotify(NAV_RIGHT, SPECIAL_ME));
}

/*! \brief hang up the active line */
void PopcAastra::hangup()
{
    // qDebug() << Q_FUNC_INFO;
    emit ipbxCommand(getAastraKeyNotify(GOODBYE, SPECIAL_ME));
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
    // qDebug() << Q_FUNC_INFO;
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
    // qDebug() << Q_FUNC_INFO << number;
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
    // qDebug() << Q_FUNC_INFO << text;
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
    // qDebug() << Q_FUNC_INFO;

    m_contact_completer = new FilteredCompleter(this);
    m_contact_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_contact_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

    m_targets->setCompleter(m_contact_completer);
}

/*! \brief destructor
 */
PopcAastra::~PopcAastra()
{
    // qDebug() << Q_FUNC_INFO;
}

