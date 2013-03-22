#include "agent_status_delegate.h"

AgentStatusDelegate::AgentStatusDelegate()
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
