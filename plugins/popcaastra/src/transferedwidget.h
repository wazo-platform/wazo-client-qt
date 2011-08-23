#ifndef __TRANSFEREDWIDGET_H__
#define __TRANSFEREDWIDGET_H__

#include "pendingwidget.h"

class TransferedWidget: public PendingWidget
{

Q_OBJECT

public:
    TransferedWidget(const QString &, const QString &,
                     const QString &, const QString &, QWidget *);
    void update();
    bool toRemove() const;
public slots:
    void doPickup();
signals:
    void pickup(const QString &);
protected:
    void buildui();
private:
    QString m_called_num;
    QString m_t_name;
    QString m_t_num;
};

#endif // __TRANSFEREDWIDGET_H__
