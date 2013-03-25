#include <QPainter>
#include <QLabel>

#include <xletlib/agents_model.h>

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
    const QAbstractItemModel * model = index.model();
    QModelIndex agent_firstname_index = model->index(index.row(), AgentsModel::FIRSTNAME);
    QString agent_firstname = model->data(agent_firstname_index).toString();
    QModelIndex agent_lastname_index = model->index(index.row(), AgentsModel::LASTNAME);
    QString agent_lastname = model->data(agent_lastname_index).toString();
    QModelIndex agent_status_label_index = model->index(index.row(), AgentsModel::STATUS_LABEL);
    QString agent_status_label = model->data(agent_status_label_index).toString();
    QModelIndex agent_status_since_index = model->index(index.row(), AgentsModel::STATUS_SINCE);
    QString agent_status_since = model->data(agent_status_since_index).toString();
    QModelIndex agent_availability_index = model->index(index.row(), AgentsModel::AVAILABILITY);
    QColor agent_availability_color = model->data(agent_availability_index, Qt::BackgroundRole).value<QColor>();

    QWidget & widget = m_widget_storage.getWidget(index);
    QLabel * agent_name_label = widget.findChild<QLabel *>("agent_name_label");
    agent_name_label->setText(QString("%1 %2").arg(agent_firstname, agent_lastname));
    QLabel * agent_status_label_label = widget.findChild<QLabel *>("agent_status_label");
    agent_status_label_label->setText(agent_status_label);
    QLabel * agent_status_since_label = widget.findChild<QLabel *>("agent_status_since_label");
    agent_status_since_label->setText(agent_status_since);
    widget.setStyleSheet(QString("#AgentStatus {border:10px solid %1;}").arg(agent_availability_color.name()));

    QPoint offset = option.rect.topLeft();
    widget.render(painter, painter->deviceTransform().map(offset));
}

QSize AgentStatusDelegate::sizeHint(const QStyleOptionViewItem & /*style*/, const QModelIndex & index) const
{
    QWidget & widget = m_widget_storage.getWidget(index);
    return widget.size();
}
