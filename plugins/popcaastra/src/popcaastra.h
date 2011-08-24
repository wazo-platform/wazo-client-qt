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

#ifndef __POPCAASTRA_H__
#define __POPCAASTRA_H__

#include <QObject>

#include "xlet.h"

class IncomingWidget;
class FilteredLineEdit;
class FilteredCompleter;
class QCompleter;
class QHBoxLayout;
class QPushButton;
class QVBoxLayout;
class PendingWidget;

/*! \brief POPC xlet for Aastra phones
 *
 * This xlet allow the operator to "drive" his phone from an xlet using SIP
 * notify messages that are available to Aastra
 *
 * Since we cannot retrieve the state from the phone, we have to guess some
 * information about the state of the phone at some moments. This causes some
 * limitations, for example, a toggle line function would not work since we are
 * not keeping track of the active line.
 */
class PopcAastra : public XLet
{
    Q_OBJECT
public:
    PopcAastra(QWidget *parent=0);
    ~PopcAastra();
    int findFirstAvailableLine() const;
    void debugIncomingCalls() const;
protected:
    void timerEvent(QTimerEvent *);
    /*! \brief Find a matching device for a called number */
    const PhoneInfo * findCalledDevice(const QString &);
    void removeIncomingCall(const QString & xChannelId);
    void removePendingCall(const QString &);
    void removeCompletedPendings();
    void removeDefunctWidgets();
    void schedule_removal(const QString &);
private:
    /*! \brief starts tracking a number after a transfer */
    void trackTransfer(const QString &, const QString &,
                       const QString &, const QString &);
    void trackHolded(const QString &, int);
    void trackParked(const QString &, const QString &);
public slots:
    /*! \brief Dial a number using sip notify */
    void dial(const QString &);
    void updateDisplay();
    /*! \brief turns up the volume of the phone */
    void volUp();
    /*! \brief turns down the volume of the phone */
    void volDown();
    void navRight();
    void updatePhoneStatus(const QString &);
    void updateUserStatus(const QString &);
    void updateChannelStatus(const QString &);
    void updateMeetmesConfig(const QString &);
    void confLine(int, const QString &);
    void hangup();
    void hangUpLine(int);
    void holdLine(const QString &, int);
    void attendedTransfer(int);
    void blindTransfer(const QString &, int, const QString &, const QString &);
    void parkcall(int, const QString &, const QString &);
    void selectLine(int);
    void prgkey1();
    /*! \brief intercepts the call to this exten */
    void doIntercept(const QString &);
    /*! \brief receives numbers from a selected peer/contact in other xlets */
    void receiveNumberList(const QStringList &);
    /*! \brief Receives a number from other xlets */
    void receiveNumber(const QString &);
    /*! \brief Removes a pending call widget */
    void remove_pending(unsigned int);
    /*! \brief receive the text from the target field when it changes */
    void targetChanged(const QString &);
private:
    QStringList m_my_lines; //!< Our lines (SIP/abc)
    QStringList m_to_remove;
    void fillCompleter();
    QHash<QString, IncomingWidget *> m_incomingcalls;  //!< List of IncomingWidget
    QHash<QString, PendingWidget *> m_pendingcalls; //!< List of transfered calls
    QVBoxLayout * m_layout;
    QHBoxLayout * m_top_widget; //!< Container for buttons and other widgets
    QString m_selected_number;  //!< Number holding the selection in other widgets
    int m_timerid;
    int m_deltasec;
    QPushButton * m_btn_hangup;
    QPushButton * m_btn_nav_right;
    QPushButton * m_btn_vol_down;
    QPushButton * m_btn_vol_up;
    FilteredLineEdit * m_targets;
    FilteredCompleter * m_contact_completer;
};

#endif /* __POPCAASTRA_H__ */

