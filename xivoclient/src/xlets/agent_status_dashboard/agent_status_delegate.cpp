#include "agent_status_widget_storage.h"
#include "agent_status_delegate.h"

AgentStatusDelegate::AgentStatusDelegate(AgentStatusWidgetStorage & widget_storage)
    : m_widget_storage(widget_storage)
{
}

AgentStatusDelegate::~AgentStatusDelegate()
{
}

void AgentStatusDelegate::paint(QPainter * painter, const QStyleOptionViewItem & style, const QModelIndex & index) const
{
}

QSize AgentStatusDelegate::sizeHint(const QStyleOptionViewItem & style, const QModelIndex & index) const
{
    return QSize();
}
