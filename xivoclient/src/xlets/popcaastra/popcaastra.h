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

/**
 * \file popcaastra.h
 * \brief Definition of the POPC Aastra xlet
 */

#ifndef __POPCAASTRA_H__
#define __POPCAASTRA_H__

#include <QObject>

#include <baseengine.h>
#include <xlet.h>

class IncomingWidget;
class FilteredLineEdit;
class FilteredCompleter;
class QCompleter;
class QHBoxLayout;
class QPushButton;
class QVBoxLayout;
class PendingWidget;
class CurrentCallWidget;

/**
 * \brief POPC xlet for Aastra phones
 *
 * This xlet allow the operator to "drive" his phone from an xlet using SIP
 * notify messages that are available to Aastra
 *
 * Since the phone cannot send back his state to the client, we have to make
 * some assumptions about the current state of the phone. The currently active
 * line for example has to be guessed.
 *
 * This Xlet uses Aastra XML API via the CTI server and the AMI to avoir using
 * the currently used ami actions.
 */
class PopcAastra : public XLet
{

    Q_OBJECT

public:
    /**
     * Constructor of a PopcAastra Xlet
     *
     * \param parent The parent widget of the Xlet
     */
    PopcAastra(QWidget * parent=0);
protected:
    /**
     * Method to gather actions that have to be done regularly
     *
     * \param event Unused
     */
    void timerEvent(QTimerEvent *);
    /**
     * Removes an IncomingWidget from the incoming call list
     *
     * \param key The key of the key to remove
     */
    void removeIncomingCall(const QString &);
    /**
     * Removes a PendingWidget from the pending calls list
     *
     * \param key The key of the call to remove
     */
    void removePendingCall(const QString &);
    /**
     * Remove completed pending calls from the pending calls list
     *
     * Iterate thru the list of pending calls, checking if they are ready to be
     * removed. Removes each pending calls that are completed
     */
    void removeCompletedPendings();
    /**
     * Remove completed calls widget
     *
     * Check the current call and widgets that are scheduled for removal and
     * delete them
     */
    void removeDefunctWidgets();
    /**
     * Finds the first line available to place this channel
     *
     * The goal is to know/guess on which line a call is to avoid
     * mistakes when doing operations to a call since we are dealing
     * with lines on the phone device when using aastra xml api
     *
     * \return The next line available or -1
     */
    int nextLine() const;
private:
    /**
     * Track a call after a blind transfer
     *
     * Calls are tracked after blind transfer to allow the interception of
     * unanswered calls.
     * Tracked calls are added to the m_pendingcalls with other tracked widgets
     *
     * \param pxid The phone's XiVO id
     */
    void trackTransfer(const QString &);
    /**
     * Track a call on hold
     *
     * Track held calls to be able to unhold them
     *
     * \param phonexid The phone's XiVO id
     * \param line The line number on the phone
     */
    void trackHolded(const QString &, int);
    /**
     * Track parked calls
     *
     * Parked calls are tracked in m_pendingcalls to be able to unpark them if
     * no other users retrieves the call
     *
     * \param parkingxid The parking's XiVO id
     * \param phonexid The parked phone's XiVO id
     */
    void trackParked(const QString &, const QString &);
public slots:
    /**
     * Dial a number using Aastra SIP notify
     *
     * \param number The number to dial
     */
    void dial(const QString &);
    /**
     * Turns up the volume of the phone
     */
    void volUp();
    /**
     * Turns down the volume of the phone
     */
    void volDown();
    /**
     * Press the navigation right button of the device
     *
     * This button is usually used to confirm or proceed on an Aastra 6731i
     */
    void navRight();
    /**
     * Trigger widget updates when phone status are changed
     *
     * \param xphoneid The updated phone's XiVO id
     */
    void updatePhoneStatus(const QString &);
    /**
     * Add new users to the completion list for transfer targets
     *
     * \param userxid The user's XiVO id
     */
    void updateUserConfig(const QString &);
    /**
     * Add and update call widgets
     *
     * Parse incoming channel update to handle or ignore updates. When updates
     * are handled, widgets are created or updated depending on the context.
     *
     * \param cxid The channel's XiVO id
     */
    void updateChannelStatus(const QString &);
    /**
     * Update the target list with the available conference rooms
     *
     * \param mxid The meetme's XiVO id
     */
    void updateMeetmesConfig(const QString &);
    /**
     * Transfers the current call to a conference room
     *
     * If multiple conference rooms are available on the XiVO, the operator will
     * be asked for a conference room to send to caller to.
     *
     * \see promptMeetme
     */
    void conf();
    /**
     * Hang up the active line
     */
    void hangup();
    /**
     * Put the current call on hold
     */
    void hold();
    /**
     * Hang up a specified line on our phone
     *
     * \param line The phone's line to hang up
     */
    void hangupLine(int);
    /**
     * Put a specified line on hold
     *
     * \param device_identity The phone's protocol/id needed for tracking
     * \param line The phone's line to hangup
     */
    void holdLine(const QString &, int);
    /**
     * Do an attended transfer to the selected number
     */
    void attendedTransfer();
    /**
     * Do a direct transfer to the selected number
     */
    void transfer();
    /**
     * Park the selected number
     *
     * If more than one parking is available, the operator will be prompted for
     * for a target parking to send the call
     */
    void park();
    /**
     * Select a line on the phone
     *
     * \param line
     */
    void selectLine(int);
    /**
     * Simulates a press on the programmable button 1
     */
    void prgkey1();
    /**
     * Intercept a call ringing at the given number
     *
     * Sends a *8 exten on the phone to intercept a call ringing on a given
     * exten
     *
     * \param exten The exten number to intercept
     */
    void doIntercept(const QString &);
    /**
     * Receive a list of numbers for a selected peer or contact
     *
     * \param numbers The number list
     */
    void receiveNumberList(const QStringList &);
    /**
     * Receive a number from another xlet
     *
     * Receive this number from other xlet and set the selection field to this
     * value.
     *
     * \param number The received number
     */
    void receiveNumber(const QString &);
    /**
     * Removes a pending call from m_pendingcalls
     *
     * \param key The pending call key
     */
    void remove_pending(unsigned int);
    /**
     * Removes an incoming call from m_incoming
     *
     * \param id The pending call's id
     * \see PendingCall::id
     */
    void remove_incoming(int);
    /**
     * Removes the current call from the xlet
     */
    void remove_current_call();
    /**
     * Receive changes from the target field
     *
     * This event is received when the user type data in the field or when the
     * value is changed programmatically
     *
     * \param text The new value
     */
    void targetChanged(const QString &);
    /**
     * Prompt the operator for a parking
     * 
     * \return the parkinglot XiVO id
     */
    QString promptParking() const;
    /**
     * Prompt the operator for a meetme
     *
     * \return The meetme XiVO id
     */
    QString promptMeetme() const;
    /**
     * Set the completion list for the completion targets
     *
     * The completer should contain peers, contacts, conference and parkings
     */
    void fillCompleter();
private:
    /** Keys for pending calls waiting for removal */
    QStringList m_pending_to_remove;
    /** Keys for incoming calls waiting for removal */
    QStringList m_incoming_to_remove;
    /** Widget showing the currently ongoing call */
    CurrentCallWidget * m_current_call;
    /** Hash map of widgets for incoming calls */
    QHash<QString, IncomingWidget *> m_incomingcalls;
    /** Hash map of widget for pending calls */
    QHash<QString, PendingWidget *> m_pendingcalls;
    QVBoxLayout * m_layout; //!< The layout
    QHBoxLayout * m_top_widget; //!< Container for buttons and other widgets
    QString m_selected_number;  //!< Selected number
    int m_timerid; //!< The xlet's timer id
    QPushButton * m_btn_hangup; //!< Hang up button
    QPushButton * m_btn_nav_right; //!< Navigation right button
    QPushButton * m_btn_vol_down; //!< Volume down button
    QPushButton * m_btn_vol_up; //!< Volume up button
    FilteredLineEdit * m_targets; //!< Edit box for transfer targets
    FilteredCompleter * m_contact_completer; //!< Completion for the targets
};

#endif /* __POPCAASTRA_H__ */
