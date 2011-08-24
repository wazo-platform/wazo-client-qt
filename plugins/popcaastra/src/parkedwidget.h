#ifndef _PARKEDWIDGET_H_
#define _PARKEDWIDGET_H_

#include "pendingwidget.h"

class ParkedWidget: public PendingWidget
{

Q_OBJECT

public:
    ParkedWidget(const QString &, const QString &, QWidget *);
    void update();
    bool toRemove() const;
public slots:
    void doPickup();
signals:
    void pickup(const QString &);
protected:
    void buildui();
private:
    QString m_parking_id; //!< The parking's XiVO id
};

#endif /* _PARKEDWIDGET_H_ */
