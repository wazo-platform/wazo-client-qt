#include <QHBoxLayout>
#include <QPixmap>
#include <QDebug>

#include "delegate.h"

RemoveDelegate::RemoveDelegate(QObject *parent)
    : QItemDelegate(parent),
      m_text(NULL),
      m_button(NULL)
{
    qDebug() << Q_FUNC_INFO;
    m_text = new QLabel();
    m_button = new QLabel();
    connect(this, SIGNAL(closeEditor(QWidget *)),
            this, SLOT(closed()));
}

QWidget * RemoveDelegate::createEditor(QWidget *parent,
     const QStyleOptionViewItem &/* option */,
     const QModelIndex &/* index */) const
{
    qDebug() << Q_FUNC_INFO;
    QWidget *w = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout();
    w->setLayout(layout);
    
    layout->addWidget(m_text);
    
    m_button->setMouseTracking(true);
    m_button->setPixmap(QPixmap("gtk-close.png"));
    layout->addWidget(m_button);
    return w;
}

void RemoveDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    qDebug() << Q_FUNC_INFO;
    // m_text->setText(index.model()->data(index, Qt::EditRole).toString());
    m_text->setText("abc");
}

void RemoveDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

void RemoveDelegate::closed()
{
    qDebug() << Q_FUNC_INFO;
}
