#ifndef __AGENT_STATUS_DELEGATE__
#define __AGENT_STATUS_DELEGATE__

#include <QAbstractItemDelegate>

class AgentStatusDelegate: public QAbstractItemDelegate
{
 public:
    AgentStatusDelegate();
    ~AgentStatusDelegate();
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
 private:
};

#endif
