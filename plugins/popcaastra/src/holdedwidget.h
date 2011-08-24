#ifndef _HOLDEDWIDGET_H_
#define _HOLDEDWIDGET_H_

#include "pendingwidget.h"

class HoldedWidget: public PendingWidget
{

Q_OBJECT

public:
    HoldedWidget(const QString &, int, QWidget *);
    bool toRemove() const;
    void update();
public slots:
    void doPickup();
signals:
    void pickup(int);
protected:
    void buildui();
private:
    int m_line;
};

#endif /* _HOLDEDWIDGET_H_ */
