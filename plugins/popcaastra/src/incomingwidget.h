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
        IncomingWidget(int line_num, const QString & xchannel, QWidget * parent);
        void updateWidget();
        int line() const;
        QString toString() const;
    public slots:
        void doHangUp();    //!< Hang up the line
        void doBlindTransfer();
        void doAttendedTransfer();
        void doTransferToNumber(const QString & number); //!< Transfer to number
        void doParkCall();  //!< Park the call
    protected:
        void timerEvent(QTimerEvent *);
        void contextMenuEvent(QContextMenuEvent *);
        void mousePressEvent(QMouseEvent *);
    private:
        void setActionPixmap();
        void updateCallTimeLabel();
    signals:
        void doHangUp(int);     //!< hang up a line
        void doBlindTransfer(int, const QString &, const QString &);
        void doAttendedTransfer(int);
        void selectLine(int);
        void doParkCall(int);
    private:
        int m_line;
        QString m_xchannel;
        QString m_peer_name;
        QString m_peer_number;
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
        double m_start;
        QAction * m_hangUpAction;
        QAction * m_attendedTransferAction;
        QAction * m_blindTransferAction;
        QAction * m_parkCallAction;
};

#endif
