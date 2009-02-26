#include <QDebug>
#include <QPainter>
#include <QLabel>
#include "externalphonepeerwidget.h"
#include "externalphonedialog.h"

ExternalPhonePeerWidget::ExternalPhonePeerWidget(BaseEngine * engine, const QVariant & options, const QString & label, const QString & number)
    : BasePeerWidget(engine, 0, options)
{
    m_number = number;
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
    setText( label );
    setToolTip( number );
    m_editable = true;  // allow "edit" option in context menu
}

void ExternalPhonePeerWidget::setName(const QString & name)
{
    setText( name );
}

QString ExternalPhonePeerWidget::name() const
{
    return m_text;
}

void ExternalPhonePeerWidget::setText(const QString & text)
{
    m_text = text;
    // calculate size
    QFontMetrics fontMetrics( font() );
    QSize size = fontMetrics.size(0, m_text);
    // set a maximum width PeerWidget
    if(size.width() > maxWidthWanted())
        size.setWidth(maxWidthWanted());
    setMinimumSize( size );
    update();
}

/*! \brief paint this widget
 *
 * Fill the whole widget with a rectange of color
 * which indicate the status of the phone.
 * Draw a small square for user presence indicator
 * Then write the name of the user.
 */
void ExternalPhonePeerWidget::paintEvent(QPaintEvent *event)
{
    QRect rectangle = contentsRect();
    QPainter painter( this );
    // draw the color rectangle
    painter.setBrush( QColor(192, 160, 160) );
    painter.setPen( Qt::NoPen );
    painter.drawRect( rectangle );
    // write the text
    painter.setPen(Qt::SolidLine);
    painter.drawText( rectangle, Qt::AlignVCenter | Qt::AlignHCenter, m_text );
}

/*! \brief 
 *
 *
 */
void ExternalPhonePeerWidget::edit()
{
    ExternalPhoneDialog dialog;
    dialog.setNumber( m_number );
    dialog.setLabel( m_text );
    if(dialog.exec())
    {
        if(!dialog.number().isEmpty())
            m_number = dialog.number();
        if(!dialog.label().isEmpty())
            setText( dialog.label() );
    }
}

