#include <QPainter>

#include "agent_status_widget_storage.h"
#include "agent_status_delegate.h"

AgentStatusDelegate::AgentStatusDelegate(AgentStatusWidgetStorage & widget_storage)
    : m_widget_storage(widget_storage)
{
}

AgentStatusDelegate::~AgentStatusDelegate()
{
}

void AgentStatusDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QWidget & widget = m_widget_storage.getWidget(index);
    QPoint offset = option.rect.topLeft();
    widget.render(painter, painter->deviceTransform().map(offset));
}

QSize AgentStatusDelegate::sizeHint(const QStyleOptionViewItem & style, const QModelIndex & index) const
{
    QWidget & widget = m_widget_storage.getWidget(index);
    return widget.size();
}
