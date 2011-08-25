#ifndef __INCOMINGWIDGET_H__
#define __INCOMINGWIDGET_H__

#include "pendingwidget.h"

class UserInfo;
class QPushButton;

/*! \brief A widget to display relevant information about an incoming call
 *
 *  A new widget should be created for each incoming call to the operator'S
 *  phone. The widget will display some information about this call and allow
 *  the user to interact with the call with a set of buttons.
 */
class IncomingWidget: public PendingWidget
{
    Q_OBJECT

public:
    IncomingWidget(int line, const QString &,
                   const QString &, QWidget *);
    void update();
    bool toRemove() const;
    int line() const { return m_line; };
public slots:
    void doIgnore();
    void doPickup();
protected:
    void buildui();
signals:
    void pickup(const QString &);
    void ignore(const QString &);
private:
    int m_line;
    QString m_channel_xid;
    QPushButton * m_btn_ignore;
    QPushButton * m_btn_answer;
};

#endif
