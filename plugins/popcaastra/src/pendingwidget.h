#ifndef __PENDINGWIDGET_H_
#define __PENDINGWIDGET_H_

#include <QAction>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QObject>
#include <QWidget>

/*!
 * A base widget class to track Pending calls that can be picked back by the
 * the operator.
 */
class PendingWidget : public QWidget
{
    Q_OBJECT

public:
    /*! \brief Constructor
     *
     * \param phone_xid The tracked phone's XiVO id
     * \param parent
     */
    PendingWidget(const QString &, QWidget *);
    virtual ~PendingWidget() = 0;
    /*! \brief Refresh the widgets internal data and UI */
    virtual void update() = 0;
public slots:
    /*! \brief Triggered action called when the calls is being picked up */
    virtual void doPickup() = 0;
private:
    virtual void buildui; //!< Creates inner widgets
    const QString & m_phonexid; //!< Tracked phone's XiVO id
    double m_time_transfer; //!< When we started tracking
    QLabel * m_lbl_string; //!< The info string for this call
    QPushButton * m_btn_pickup; //!< The button to pickup this call
    QHBoxLayout * m_layout; //!< The layout
};

#endif
