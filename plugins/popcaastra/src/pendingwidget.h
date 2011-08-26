#ifndef __PENDINGWIDGET_H__
#define __PENDINGWIDGET_H__

#include <QDebug>
#include <QObject>
#include <QWidget>

class QLabel;
class QPushButton;
class QHBoxLayout;

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
    virtual ~PendingWidget() {};
    /*! \brief Refresh the widgets internal data and UI */
    virtual void update() = 0;
    /*! \brief Check if this widget is ready to be removed */
    virtual bool toRemove() const = 0;
    unsigned int id() const { return m_id; };
    const QString & phonexid() const;
public slots:
    /*! \brief Triggered action called when the calls is being picked up */
    virtual void doPickup() = 0;
signals:
    /*! \brief Notify the popc xlet that this widget should be removed */
    void remove_me(unsigned int);
protected:
    virtual void buildui(bool); //!< Creates inner widgets
    void set_string(const QString &);
    QString started_since() const;
    double time_start() const { return m_time_transfer; };
    QHBoxLayout * layout() { return m_layout; };
private:
    unsigned int m_id; //!< This call's internal id
    QString m_phonexid; //!< Tracked phone's XiVO id
    double m_time_transfer; //!< When we started tracking
    QHBoxLayout * m_layout; //!< The layout
    QLabel * m_lbl_string; //!< The info string for this call
    QPushButton * m_btn_pickup; //!< The button to pickup this call
    static unsigned int counted; //!< Used to assign an id to each instance
};

inline const QString & PendingWidget::phonexid() const
{
    return m_phonexid;
}

#endif

