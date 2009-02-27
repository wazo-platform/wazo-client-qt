#ifndef __BASEPEERWIDGET_H__
#define __BASEPEERWIDGET_H__

#include <QWidget>
#include <QMouseEvent>

class UserInfo;
class BaseEngine;

/*! \brief Abstract class for "Peer" Widgets
 *
 */
class BasePeerWidget : public QWidget
{
    Q_OBJECT
public:
    BasePeerWidget(BaseEngine *, UserInfo *, const QVariant &);
    //! change username to be displayed
    virtual void setName(const QString &) = 0;
    //! set Tooltip for agent
    virtual void setAgentToolTip(const QString &, const QStringList &) = 0;
    //! set agent state
    virtual void setAgentState(const QString & color) = 0;
    //! update presence information displayed
    virtual void updatePresence() = 0;
    //! update phones information displayed
    virtual void updatePhonesStates() = 0;
    //! Phone number
    virtual const QString & number() const { return m_number; };
    //! Name
    virtual QString name() const;
    int maxWidthWanted() const { return m_maxWidthWanted; };
protected:
    void mouseDoubleClickEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void contextMenuEvent(QContextMenuEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dropEvent(QDropEvent *);
signals:
    //! originate, transfer, etc...
    void actionCall(const QString &,
                    const QString &,
                    const QString &dst = "");
    //! hide the widget from the containing window
    void removeFromPanel();
protected slots:
    void dial();
    void hangup();
    void intercept();
    void transfer();
    void itransfer();
    void parkcall();
protected:
    BaseEngine * m_engine;      //!< Xivo CTI Client Engine
    UserInfo * m_ui;            //!< user infos
    QPoint m_dragstartpos;            //!< drag start position
    QAction * m_dialAction;     //!< action to dial this number
    QAction * m_removeAction;   //!< action to remove this peer from the window
    QString m_number;   //!< phone number (filled if m_ui is NULL)
    bool m_editable;    //!< editable from a user point of view
private:
    int m_maxWidthWanted;   //!< maximum width for "small" items
};

#endif

