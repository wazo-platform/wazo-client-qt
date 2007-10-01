#ifndef __FAXPANEL_H__
#define __FAXPANEL_H__

/* $Id$ */

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
private:
	QWidget * m_mainwindow;	//!< MainWidget where some parameters are commited to
        QLineEdit * m_openFileNameLabel;
	QLineEdit * m_destination;
        QCheckBox * m_maskornot;
        QPushButton * m_sendButton;
        BaseEngine * m_engine;
        DirDialog * m_dirw;
};

#endif
