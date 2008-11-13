/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
*/

/* $Revision$
 * $Date$
 */

#ifndef __CALLCAMPAIGNPANEL_H__
#define __CALLCAMPAIGNPANEL_H__

#include <QHash>
#include <QObject>
#include <QVariant>
#include <QWidget>

class QGridLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QVBoxLayout;

class UserInfo;

/*! \brief CallCampaign Panel
 */
class CallCampaignPanel : public QWidget
{
	Q_OBJECT
public:
	CallCampaignPanel(QWidget * parent = 0);
signals:
        void requestFileList(const QString &);
public slots:
        void setGuiOptions(const QVariant &);
        void setUserInfo(const UserInfo *);
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
        
        const UserInfo * m_ui;
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
