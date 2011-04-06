#ifndef __INCOMINGWIDGET_H__
#define __INCOMINGWIDGET_H__

#include <QObject>
#include <QWidget>

#include "popcaastra.h"

class QHBoxLayout;
class QLabel;
class UserInfo;

class IncomingWidget : public QWidget
{
    Q_OBJECT

    public:
        IncomingWidget(int line_num, const UserInfo *, const QString & xchannel, QWidget * parent);
        void updateWidget();
        int line() const;
        QString toString() const;
    public slots:
        void doHangUp();    //!< Hang up the line
        void doTransferToNumber(const QString & number); //!< Transfer to number
        void doParkCall();  //!< Park the call
    protected:
        void timerEvent(QTimerEvent *);
        void contextMenuEvent(QContextMenuEvent *);
    private:
        void setActionPixmap();
        void updateCallTimeLabel();
    signals:
        void doHangUp(int);     //!< hang up a line
    private:
        const UserInfo * m_uinfo;
        int m_line;
        QString m_xchannel;
        QHBoxLayout * m_layout;
        QLabel * m_lbl_line;
        QLabel * m_lbl_name;
        QLabel * m_lbl_exten;
        QLabel * m_lbl_time;
        QLabel * m_lbl_direction;
        QLabel * m_lbl_status;
        QPixmap m_image;
        bool m_parkedCall;
        bool m_holdedCall;
        QAction * m_hangUpAction;
        QAction * m_transferToNumberAction;
        QAction * m_parkCallAction;
};

#endif
