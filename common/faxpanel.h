/* XIVO CTI clients
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Revision$
 * $Date$
 */

#ifndef __FAXPANEL_H__
#define __FAXPANEL_H__

#include <QWidget>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

class BaseEngine;
class DirDialog;
class MainWidget;

class FaxPanel : public QWidget
{
	Q_OBJECT
public:
	FaxPanel(BaseEngine *, QWidget * parent = 0);
        ~FaxPanel();
signals:
        void faxSend(const QString &, const QString &, Qt::CheckState);
public slots:
        void setOpenFileName();
        void sendFax();
        void dirLookup();
        void popupMsg(const QString &);
        void destNumberChanged(const QString &);
        void fileNameChanged(const QString &);
private:
	QWidget * m_mainwindow;	//!< MainWidget where some parameters are commited to
        QLineEdit * m_openFileNameLabel;
	QLineEdit * m_destination;
        QCheckBox * m_maskornot;
        QPushButton * m_sendButton;
        BaseEngine * m_engine;
        DirDialog * m_dirw;

        QString m_dest_string;
        QString m_file_string;
};

#endif
