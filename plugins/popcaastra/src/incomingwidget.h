#ifndef __INCOMINGWIDGET_H__
#define __INCOMINGWIDGET_H__

#include "popcaastra.h"

class QHBoxLayout;
class QLabel;
class QObject;
class QPushButton;
class QSize;
class QWidget;
class UserInfo;

/*! \brief A widget to display relevant information about an incoming call
 *
 *  A new widget should be created for each incoming call to the operator'S
 *  phone. The widget will display some information about this call and allow
 *  the user to interact with the call with a set of buttons.
 */
class IncomingWidget : public QWidget
{
    Q_OBJECT

    public:
        /*! \brief The IncomingWidget constructor
         *  \param line The line on the phone (L1, L2, etc)
         *  \param xchan The Xivo channel of this call
         *  \param parent The parent widget
         */
        IncomingWidget(int line_num, const QString & xchan, QWidget * parent);
        ~IncomingWidget();
        /*! \brief Get the phone's line number for this call */
        int line() const { return m_line; }
        QString toString() const;
        /*! \brief Update and refresh the widget */
        void updateWidget();
        const QString xChannelId() const { return m_xchannel; }
    public slots:
        void doHangUp();    //!< Hang up the line
        void doBlindTransfer();
        void doAttendedTransfer();
        void doTransferToNumber(const QString & number); //!< Transfer to number
        void doParkCall();  //!< Park the call
    protected:
        /*! \brief Build the layout of the widget */
        void buildLayout();
        void mousePressEvent(QMouseEvent *);
        /*! \brief Refresh the UI to reflect a change in the data */
        void refreshUI();
        void timerEvent(QTimerEvent *);
        /*! \brief Updates the widgets informations from a channel id
         *  \param xcid Channel key */
        void updateFromChannelInfo(const QString &);
    private:
        void updateCallTimeLabel();
        /*! \brief Register signals and slots */
        void setSignalsSlots();
    signals:
        void doHangUp(int);     //!< hang up a line
        void doBlindTransfer(int, const QString &, const QString &);
        void doAttendedTransfer(int);
        void selectLine(int);
        void doParkCall(int);
    private:
        int m_line;                 //!< The phone's line number
        QString m_xchannel;
        QString m_peer_name;
        QString m_peer_number;
        const UserInfo * m_peer;
        QHBoxLayout * m_layout;
        QLabel * m_lbl_line;
        QLabel * m_lbl_name;
        QLabel * m_lbl_exten;
        QLabel * m_lbl_time;
        bool m_parkedCall;
        bool m_holdedCall;
        double m_start;
        QPushButton * m_btn_hangup;
        QPushButton * m_btn_hold;
        QPushButton * m_btn_park;
        QPushButton * m_btn_atxfer;
        QPushButton * m_btn_xfer;
        QPushButton * m_btn_conf;
        QSize * m_small_button_sz;  // Default size for the buttons
};

#endif
