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

#ifndef __CALLCAMPAIGNPANEL_H__
#define __CALLCAMPAIGNPANEL_H__

#include <QHash>
#include <QObject>
#include <QVariant>
#include "xlet.h"

class QGridLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QVBoxLayout;

class UserInfo;

/*! \brief CallCampaign Panel
 */
class CallCampaignPanel : public XLet
{
    Q_OBJECT
public:
    CallCampaignPanel(BaseEngine * engine, QWidget * parent = 0);
signals:
    void requestFileList(const QString &);
public slots:
    void setGuiOptions(const QVariant &) {};
    void setUserInfo(const UserInfo *) {};
    void setOpenFileName();
    void fileNameChanged(const QString &);
    void getCalls();
    void startCalls();
    void stopCalls();
    void clearCalls();
    void loadFileClicked();
    void requestFileListResult(const QVariant &);
private:
    void addNumber(const QString &);
    void checkStatuses();
    
    QLineEdit * m_openFileNameLabel;
    QVBoxLayout * m_vlayout;
    QHBoxLayout * m_hlayout1;
    QHBoxLayout * m_hlayout2;
    QGridLayout * m_glayout;
    QHash<QString, QString> m_numbers;
    QHash<QString, QLabel *> m_numlabel;
    QHash<QString, QPushButton *> m_qpbstart;
};

#endif /* __CALLCAMPAIGNPANEL_H__ */

