#ifndef __TRANSFEREDWIDGET_H_
#define __TRANSFEREDWIDGET_H_

#include <QAction>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
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
        ~TransferedWidget();
        /*! \brief refresh the widget */
        void updateWidget();
        /*! \brief returns true if the widget can be removed from the list */
        bool readyToBeRemoved() const;
        /*! \brief returns the exten that this call has been transfered to */
        const QString & number() const;
    public slots:
        void doIntercept(); /*! \brief intercept this transfer */
    signals:
        void intercept(const QString &);/*! \brief sends an intercept request */
    protected:
        void contextMenuEvent(QContextMenuEvent *);
    private:
        const QString m_number;         /*!< transfering to this number */
        double m_time_transfer;         /*!< transfer time */
        QHBoxLayout * m_layout;         /*!< Container layout */
        QLabel * m_lbl_status;          /*!< the callers phone status */
        QLabel * m_lbl_transfered_name; /*!< the transfered person's name */
        QLabel * m_lbl_transfered_number;/*!< the transfered person's number */
        QLabel * m_lbl_called_name;     /*!< the called person's name */
        QLabel * m_lbl_called_number;   /*!< the called person's number */
        /*! time elapsed between the start of the transfer and the last update */
        QLabel * m_lbl_time;
        QString m_called_phone_id;      /*!< xphoneid of the called */
        QString m_hintstatus;
        bool m_readyToBeRemoved;
        QAction * m_interceptAction;    /*!< Intercept a transfered call */

        void setXphoneId();
};

#endif
