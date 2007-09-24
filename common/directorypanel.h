/* XIVO switchboard
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

/* $Id$ */

#ifndef __DIRECTORYPANEL_H__
#define __DIRECTORYPANEL_H__

#include <QWidget>

class QDropEvent;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;

class ExtendedLineEdit;
class ExtendedTableWidget;
class PeerChannel;

/*! \brief Directory allowing search
 */
class DirectoryPanel : public QWidget
{
	Q_OBJECT
public:
	DirectoryPanel(QWidget * parent = 0);
protected:
        void dropEvent ( QDropEvent * );
signals:
	//! start a search
	void searchDirectory(const QString &);
	//! dial selected number
	void emitDial(const QString &);
	void copyNumber(const QString &);
	//! transfer one of my call to this number
	void transferCall(const QString &, const QString &);
	//! originate a call
	void originateCall(const QString &, const QString &);
private slots:
	void dialNumber();
        void sendMail();
	void startSearch();
        void itemClicked(QTableWidgetItem *);
	void itemDoubleClicked(QTableWidgetItem *);
	void transferChan(const QString &);
public slots:
        void contextMenuEvent(QContextMenuEvent *);
	void setSearchResponse(const QString &);
	void stop();
	void updateMyCalls(const QStringList &, const QStringList &, const QStringList &);
private:
	ExtendedLineEdit * m_searchText;	//!< search text input
	ExtendedTableWidget * m_table;		//!< table to display results
	QPushButton * m_searchButton;	//!< button
	QString m_numberToDial;		//!< used to store number to dial or to transfer to
	QString m_mailAddr;		//!< used to store email address
	QList<PeerChannel *> m_mychannels;	//!< "my channels" list for transfer menu
};

#endif
