#include <QDebug>
#include <QKeyEvent>
#include <typeinfo>

#include "completionedit.h"

FilteredCompleter::FilteredCompleter(const QStringList & list, QObject * parent)
    : QCompleter(parent), m_list(list), m_model()
{
    setModel(&m_model); // Default is QStringListModel
}

void FilteredCompleter::update(const QString & query)
{
    QStringList filtered = m_list.filter(query, caseSensitivity());
    m_model.setStringList(filtered);
    m_word = query;
    complete(); // Start auto completion
}

const QString FilteredCompleter::word() const
{
    return m_word;
}

FilteredLineEdit::FilteredLineEdit(QWidget *parent)
    : QLineEdit(parent), c(0)
{
}

void FilteredLineEdit::setCompleter(QCompleter * completer)
{
    qDebug() << Q_FUNC_INFO;

    if (c)
        QObject::disconnect(c, 0, this, 0);

    c = completer;
    c->setCompletionMode(QCompleter::PopupCompletion);

    if (!c)
        return;

    c->setWidget(this);
    connect(completer, SIGNAL(activated(const QString &)),
            this, SLOT(insertCompletion(const QString &)));
}

const QCompleter * FilteredLineEdit::completer() const
{
    qDebug() << Q_FUNC_INFO;

    return c;
}

void FilteredLineEdit::insertCompletion(const QString & completion)
{
    qDebug() << Q_FUNC_INFO;

    setText(completion);
    selectAll();
}

void FilteredLineEdit::keyPressEvent(QKeyEvent * e)
{
    qDebug() << Q_FUNC_INFO;

    if (c && c->popup() && c->popup()->isVisible())
    {
        // The following keys are forwarded by the completer to the widget
        switch (e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // Let the completer do default behavior
        }
    }

    bool isShortcut = (e->modifiers() & Qt::ControlModifier)
            && e->key() == Qt::Key_E;
    // Don't send the shortcut (CTRL-E) to the text edit
    if (!isShortcut)
        QLineEdit::keyPressEvent(e);

    if (!c)
        return;

    bool ctrlOrShift = e->modifiers() &
            (Qt::ControlModifier | Qt::ShiftModifier);
    if (!isShortcut && !ctrlOrShift && e->modifiers() != Qt::NoModifier)
    {
        c->popup()->hide();
        return;
    }

    try {
        (dynamic_cast<FilteredCompleter *>(c))->update(text());
    } catch (const std::bad_cast & e) {
        qDebug() << Q_FUNC_INFO << "Only FilteredCompleter are updated";
    }
    if (c && c->popup()) {
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
}

