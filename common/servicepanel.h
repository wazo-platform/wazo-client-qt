#ifndef __SERVICEPANEL_H__
#define __SERVICEPANEL_H__

/* $Id$ */

#include <QWidget>

class QCheckBox;
class QLineEdit;

class ServicePanel : public QWidget
{
	Q_OBJECT
public:
	ServicePanel(QWidget * parent = 0);
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
private slots:
	void uncondForwardToggled(bool);
	void forwardOnBusyToggled(bool);
	void forwardOnUnavailableToggled(bool);
	void toggleUncondIfAllowed(const QString &);
	void toggleOnBusyIfAllowed(const QString &);
	void toggleOnUnavailIfAllowed(const QString &);
private:
        QString m_peer;
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

#endif

