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

#ifndef __DETAILEDEXTERNALPHONEPEERWIDGET_H__
#define __DETAILEDEXTERNALPHONEPEERWIDGET_H__

#include <QLabel>
#include "basepeerwidget.h"

/*! \brief External phone widget for detailed switchboard display
 */
class DetailedExternalPhonePeerWidget : public BasePeerWidget
{
    Q_OBJECT
public:
    DetailedExternalPhonePeerWidget(BaseEngine *, const QString &, const QString &);
    void setAgentToolTip(const QString &, const QStringList &) { };
    void setAgentState(const QString &) { };
    void updatePresence() { };
    void updatePhonesStates() { };
    void setName(const QString & name) { setText(name); };
    void setText(const QString & text);
    QString name() const { return m_textlbl->text(); };
public slots:
    void edit();
private:
    QLabel * m_textlbl;         //!< text label
    QLabel * m_lblphone;        //!< phone icon
    QLabel * m_lblphonenum;     //!< phone number (text)
};

#endif

