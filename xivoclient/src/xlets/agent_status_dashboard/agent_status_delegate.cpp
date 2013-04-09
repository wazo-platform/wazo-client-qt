#include <QPainter>
#include <QLabel>
#include <QDebug>

#include <xletlib/agents_model.h>
#include <storage/agentinfo.h>

#include "agent_status_widget.h"
#include "agent_status_widget_storage.h"
#include "agent_status_delegate.h"

AgentStatusDelegate::AgentStatusDelegate(AgentStatusWidgetStorage * widget_storage)
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
    QModelIndex agent_availability_text_index = model->index(index.row(), AgentsModel::STATUS_LABEL);
    QString agent_availability_text = model->data(agent_availability_text_index).toString();
    QModelIndex agent_status_since_index = model->index(index.row(), AgentsModel::STATUS_SINCE);
    QString agent_status_since = model->data(agent_status_since_index).toString();
    QModelIndex agent_availability_index = model->index(index.row(), AgentsModel::AVAILABILITY);
    QString agent_availability = model->data(agent_availability_index, Qt::UserRole).toString();

    QString formatted_agent_name = (QString("%1 %2").arg(this->getInitials(agent_firstname), agent_lastname).left(this->agent_name_max_length));

    if(this->m_widget_storage == NULL){
        qDebug() << Q_FUNC_INFO << "Storage is NULL";
        return;
    }
    AgentStatusWidget * widget = this->m_widget_storage->getWidget(index);
    if(widget == NULL) {
        qDebug() << Q_FUNC_INFO << "Widget is NULL";
        return;
    }

    widget->setAvailabilityStyle(agent_availability);
    widget->setAgentName(formatted_agent_name);
    widget->setAvailabilityText(agent_availability_text);
    widget->setStatusSince(agent_status_since);

    QPoint offset = option.rect.topLeft();
    widget->render(painter, painter->deviceTransform().map(offset));
}

QString AgentStatusDelegate::getInitials(const QString & full_string) const
{
    QStringList separated_string = full_string.split(QRegExp("\\W+"), QString::SkipEmptyParts);
    QString return_value;
    foreach(QString word_alone, separated_string) {
        return_value.append(word_alone.left(1)).append(".");
    }

    return return_value;
}

QSize AgentStatusDelegate::sizeHint(const QStyleOptionViewItem & /*style*/, const QModelIndex & index) const
{
    if(this->m_widget_storage == NULL){
        qDebug() << Q_FUNC_INFO << "Storage is NULL";
        return QSize();
    }
    QWidget * widget = this->m_widget_storage->getWidget(index);
    if(widget == NULL) {
        qDebug() << Q_FUNC_INFO << "Widget is NULL";
        return QSize();
    }

    return widget->size();
}
