/* XIVO CTI clients
Copyright (C) 2007, 2008  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

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

#ifndef __SERVICEPANEL_H__
#define __SERVICEPANEL_H__

#include <QWidget>

class QCheckBox;
class QLineEdit;

class ServiceStatus;

class ServicePanel : public QWidget
{
	Q_OBJECT
public:
	ServicePanel(const QStringList &,
                     QWidget * parent = 0);
signals:
	void voiceMailToggled(bool);
	void callRecordingToggled(bool);
	void callFilteringToggled(bool);
	void dndToggled(bool);
	void uncondForwardChanged(bool, const QString &);
	void forwardOnBusyChanged(bool, const QString &);
	void forwardOnUnavailableChanged(bool, const QString &);
	void askFeatures(const QString &); //!< need features to be updated !
public slots:
	void setVoiceMail(bool);
	void setCallRecording(bool);
	void setCallFiltering(bool);
	void setDnd(bool);
	void setUncondForward(bool, const QString &);
	void setUncondForward(bool);
	void setUncondForward(const QString &);
	void setForwardOnBusy(bool, const QString &);
	void setForwardOnBusy(bool);
	void setForwardOnBusy(const QString &);
	void setForwardOnUnavailable(bool, const QString &);
	void setForwardOnUnavailable(bool);
	void setForwardOnUnavailable(const QString &);
        void setPeerToDisplay(const QString &);
        void Connect();
        void DisConnect();
        void Reset();
	void getRecordedStatus();
	void setRecordedStatus();
private slots:
	void uncondForwardToggled(bool);
	void forwardOnBusyToggled(bool);
	void forwardOnUnavailableToggled(bool);
	void toggleUncondIfAllowed(const QString &);
	void toggleOnBusyIfAllowed(const QString &);
	void toggleOnUnavailIfAllowed(const QString &);
private:
        ServiceStatus * m_status;
        QString m_peer;
        QStringList m_capas;
        QCheckBox * m_voicemail;
	QCheckBox * m_callrecording;
	QCheckBox * m_callfiltering;
	QCheckBox * m_dnd;
	QCheckBox * m_uncondforward;
	QLineEdit * m_uncondforwarddest;
	QCheckBox * m_forwardonbusy;
	QLineEdit * m_forwardonbusydest;
	QCheckBox * m_forwardonunavailable;
	QLineEdit * m_forwardonunavailabledest;
};


class ServiceStatus
{
 public:
        ServiceStatus();
	bool    m_voicemail;
	bool    m_callrecording;
	bool    m_callfiltering;
	bool    m_dnd;
	bool    m_uncondforward;
	QString m_uncondforwarddest;
	bool    m_forwardonbusy;
	QString m_forwardonbusydest;
	bool    m_forwardonunavailable;
	QString m_forwardonunavailabledest;
 public:
	void setVoiceMail(bool);
	void setCallRecording(bool);
	void setCallFiltering(bool);
	void setDnd(bool);
	void setUncondForward(bool, const QString &);
	void setUncondForward(bool);
	void setUncondForward(const QString &);
	void setForwardOnBusy(bool, const QString &);
	void setForwardOnBusy(bool);
	void setForwardOnBusy(const QString &);
	void setForwardOnUnavailable(bool, const QString &);
	void setForwardOnUnavailable(bool);
	void setForwardOnUnavailable(const QString &);

	void display();
};

#endif
