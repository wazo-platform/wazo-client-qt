#ifndef __AGENT_STATUS_DELEGATE__
#define __AGENT_STATUS_DELEGATE__

#include <QAbstractItemDelegate>

class AgentStatusWidgetStorage;

class AgentStatusDelegate: public QAbstractItemDelegate
{
    const static int agent_name_max_length = 11;

 public:
    AgentStatusDelegate(AgentStatusWidgetStorage & widget_storage);
    ~AgentStatusDelegate();
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

 private:
    QString getInitials(const QString & full_string) const;
    AgentStatusWidgetStorage & m_widget_storage;
};

#endif
