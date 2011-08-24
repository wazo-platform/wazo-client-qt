#include "parkedwidget.h"

#include "baseengine.h"
#include "channelinfo.h"
#include "parkinginfo.h"
#include "pendingwidget.h"
#include "phoneinfo.h"

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
    const PhoneInfo * p = b_engine->phone(phonexid());
    if (! p || ! p->xchannels().size()) {
        emit remove_me(id());
        return;
    }
    static const QString s = "%0 %1 %2 %3";
    static const QString message = tr("parked at");
    const ChannelInfo * c = b_engine->channel(p->xchannels().last());
    const QString & display = c ? c->thisdisplay() : p->number();
    set_string(QString(s)
               .arg(display).arg(message)
               .arg(get_bay()).arg(started_since()));
}

QString & ParkedWidget::get_bay()
{
    if (m_parking_bay.isEmpty()) {
        const ParkingInfo * parking = b_engine->parkinglot(m_parking_id);
        const PhoneInfo * p = b_engine->phone(phonexid());
        if (parking && p && p->xchannels().size()) {
            foreach (const QString & bay_exten, parking->parkingBays().keys()) {
                if (parking->parkingBays()[bay_exten].toMap().value("parked")
                    .toString().contains(p->identity())) {
                    m_parking_bay = bay_exten;
                    break;
                }
            }
        }
    }
    return m_parking_bay;
}

bool ParkedWidget::toRemove() const
{
    const PhoneInfo * p = b_engine->phone(phonexid());
    if (! p || ! p->xchannels().size()) return true;
    const ParkingInfo * park = b_engine->parkinglot(m_parking_id);
    if (! park ) return true;
    // parkedHere will return false for the first few seconds
    if (b_engine->timeServer() - time_start() > 5.0
        && ! park->parkedHere(p->xchannels().last())) {
        return true;
    }
    return false;
}

void ParkedWidget::doPickup()
{
    emit pickup(get_bay());
    emit remove_me(id());
}

void ParkedWidget::buildui()
{
    this->PendingWidget::buildui(true);
    connect(this, SIGNAL(pickup(const QString &)),
            parent(), SLOT(dial(const QString &)));
}
