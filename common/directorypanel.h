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

class QLineEdit;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;

/*! \brief Directory allowing search
 */
class DirectoryPanel : public QWidget
{
	Q_OBJECT
public:
	DirectoryPanel(QWidget * parent = 0);
protected:
	//void contextMenuEvent(QContextMenuEvent *);
signals:
	//! start a search
	void searchDirectory(const QString &);
	//! dial selected number
	void emitDial(const QString &);
	//! transfer one of my call to this number
	void transferCall(const QString &, const QString &);
	//! originate a call
	void originateCall(const QString &, const QString &);
	//! signal to be redirected to view
	void updateMyCalls(const QStringList &, const QStringList &, const QStringList &);
private slots:
	void startSearch();
	void itemDoubleClicked(QTableWidgetItem *);
	//void dialNumber();
public slots:
	void setSearchResponse(const QString &);
	void stop();
private:
	QLineEdit * m_searchText;	//!< search text input
	QPushButton * m_searchButton;	//!< button
	QTableWidget * m_table;		//!< table to display results
	QString m_numberToDial;		//!< used for dial action
};

#endif

