#include "holdedwidget.h"


#include "baseengine.h"
#include "channelinfo.h"
#include "phoneinfo.h"

HoldedWidget::HoldedWidget(const QString & phonexid, int line, QWidget * parent)
    : PendingWidget(phonexid, parent), m_line(line)
{
}

void HoldedWidget::update()
{
    if (!layout()) {
        buildui();
    }

    static QString base = QString("%0 %1 %2");
    static QString message = tr("on hold");
    const PhoneInfo * p = b_engine->phone(phonexid());
    if (p && p->xchannels().size()) {
        const ChannelInfo * c = b_engine->channel(p->xchannels().last());
        QString s = (QString(base)
                     .arg((c ? c->thisdisplay() : p->number()))
                     .arg(message)
                     .arg(started_since()));
        set_string(s);
    } else {
        qDebug() << Q_FUNC_INFO << "Cannot update on hold call info";
    }
}

bool HoldedWidget::toRemove() const
{
    const PhoneInfo * p = b_engine->phone(phonexid());
    if (! p || ! p->xchannels().size()) return true;

    const QString & chanxid = p->xchannels().last();
    const ChannelInfo * c = b_engine->channel(chanxid);
    if (c && c->isholded()) {
        return false;
    }
    return false;
}

void HoldedWidget::doPickup()
{
    emit pickup(m_line);
    emit remove_me(id());
}

void HoldedWidget::buildui()
{
    this->PendingWidget::buildui(true);
    connect(this, SIGNAL(pickup(int)), parent(), SLOT(selectLine(int)));
}

