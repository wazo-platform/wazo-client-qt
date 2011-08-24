#include "parkedwidget.h"

#include "parkinginfo.h"
#include "pendingwidget.h"

ParkedWidget::ParkedWidget(const QString & phonexid,
                           const QString & parkingxid,
                           QWidget * parent)
    : PendingWidget(phonexid, parent), m_parking_id(parkingxid)
{
    this->buildui();
    this->update();
}

void ParkedWidget::update()
{
    qDebug() << Q_FUNC_INFO;
}

bool ParkedWidget::toRemove() const
{
    qDebug() << Q_FUNC_INFO;
    return false;
}

void ParkedWidget::doPickup()
{
    qDebug() << Q_FUNC_INFO;
}

void ParkedWidget::buildui()
{
    qDebug() << Q_FUNC_INFO;
    this->PendingWidget::buildui();
    connect(this, SIGNAL(doPickup(const QString &)),
            parent(), SLOT(dial(const QString &)));
}
