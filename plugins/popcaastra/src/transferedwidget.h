#ifndef __TRANSFEREDWIDGET_H_
#define __TRANSFEREDWIDGET_H_

#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QWidget>

/*!
 * A widget class to show transfered calls that are not yet answered
 */
class TransferedWidget : public QWidget
{
    Q_OBJECT

    public:
        /*! \brief constructor */
        TransferedWidget(QString, const QString &, const QString &, QWidget *);
        /*! \brief refresh the widget */
        void updateWidget();
    public slots:
    signals:
    protected:
    private:
        const QString m_number;         /*!< transfering to this number */
        QString m_xchannel;             /*!< transfered current channel */
        double m_time_transfer;         /*!< transfer time */
        QHBoxLayout * m_layout;         /*!< Container layout */
        QLabel * m_lbl_status;          /*!< the callers phone status */
        QLabel * m_lbl_transfered_name; /*!< the transfered person's name */
        QLabel * m_lbl_transfered_number;/*!< the transfered person's number */
        QLabel * m_lbl_called_name;     /*!< the called person's name */
        QLabel * m_lbl_called_number;   /*!< the called person's number */
        /*! time elapsed between the start of the transfer and the last update */
        QLabel * m_lbl_time;
        QString m_called_phone_id;          /*!< xphoneid of the called */

        void findXphoneId();
};
    
#endif
