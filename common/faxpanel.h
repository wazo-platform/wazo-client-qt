#ifndef __FAXPANEL_H__
#define __FAXPANEL_H__

/* $Id$ */

#include <QWidget>

class QCheckBox;
class QLabel;
class QLineEdit;

class FaxPanel : public QWidget
{
	Q_OBJECT
public:
	FaxPanel(QWidget * parent = 0);
signals:
        void faxSend(const QString &, const QString &);
public slots:
        void setOpenFileName();
        void sendFax();
private:
        QLineEdit * m_openFileNameLabel;
	QLineEdit * m_destination;
};

#endif
