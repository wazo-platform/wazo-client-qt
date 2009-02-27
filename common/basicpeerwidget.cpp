#include <QDebug>
#include <QVariant>
#include <QPainter>
#include <QSizePolicy>
#include "basicpeerwidget.h"
#include "userinfo.h"
#include "phoneinfo.h"
#include "baseengine.h"
#include "xivoconsts.h"

/*! \brief Constructor
 *
 * initialize members and tooltip
 */
BasicPeerWidget::BasicPeerWidget(BaseEngine * engine, UserInfo * ui,
                                 const QVariant & options)
    : BasePeerWidget(engine, ui, options), m_color(0xcc, 0xcc, 0xcc),
     m_presenceColor(0xcc, 0xcc, 0xcc)
{
    // can grow horizontaly but not verticaly
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
    setText( ui->fullname().isEmpty() ? tr("(No callerid yet)") : ui->fullname() );
    setToolTip( ui->phonenumber() );
}

void BasicPeerWidget::setText(const QString & text)
{
    m_text = text;
    // calculate size
    QFontMetrics fontMetrics( font() );
    QSize size = fontMetrics.size(0, m_text);
    // maximum width for PeerWidget
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
void BasicPeerWidget::paintEvent(QPaintEvent *event)
{
    QRect rectangle = contentsRect();
    QPainter painter( this );
    // draw the color rectangle
    painter.setBrush( m_color );
    painter.setPen( Qt::NoPen );
    painter.drawRect( rectangle );
    // small square
    if(! m_ui->ctilogin().isEmpty())
    {
        painter.setBrush( m_presenceColor );
        painter.drawRect( QRect(rectangle.width() - 5, rectangle.height() - 5, 4, 4) );
    }
    // write the text
    painter.setPen(Qt::SolidLine);
    if(m_color.value() < 128)
        painter.setPen( QColor(0xcc, 0xcc, 0xcc) );
    painter.drawText( rectangle, Qt::AlignVCenter | Qt::AlignHCenter, m_text );
}

void BasicPeerWidget::setAgentToolTip(const QString & agentnum, const QStringList & queues)
{
    // do nothing !
}
 
void BasicPeerWidget::setAgentState(const QString & color)
{
    // do nothing !
}

void BasicPeerWidget::updatePresence()
{
    QString text = m_ui->phonenumber();
    if(! m_ui->ctilogin().isEmpty())
    {
        text.append(" ");
        text.append(m_ui->availstate()["longname"]);
    }
    setToolTip( text );
    m_presenceColor.setNamedColor( m_ui->availstate()["color"] );
}

void BasicPeerWidget::updatePhonesStates()
{
    // set the color according to the 1st phone
    if( !m_ui->phonelist().isEmpty() )
    {
        const PhoneInfo * pi = m_ui->getPhoneInfo(m_ui->phonelist()[0]);
        if(pi)
        {
            m_color.setNamedColor( pi->hintstatus("color") );
            update();
        }
    }
}

void BasicPeerWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
    // Call or indirect transfer
    if(event->button() == Qt::LeftButton)
    {
        qDebug() << "BasicPeerWidget::mouseDoubleClickEvent";
        // check if we are in communication
        const UserInfo * ui = m_engine->getXivoClientUser();
        if( ui && !ui->phonelist().isEmpty() )
        {
            foreach(const QString phone, ui->phonelist())
            {
                const PhoneInfo * pi = ui->getPhoneInfo( phone );
                const QMap<QString, QVariant> & comms = pi->comms();
                //qDebug() << pi->phoneid() << pi->comms();
                foreach(const QString ts, comms.keys())
                {
                    const QMap<QString, QVariant> & comm = comms[ts].toMap();
                    //qDebug() << pi->phoneid() << ts << comm;
                    const QString status = comm["status"].toString();
                    if( status == CHAN_STATUS_LINKED_CALLER || status == CHAN_STATUS_LINKED_CALLED )
                    {
                        // Initiate an indirect transfer.
                        //qDebug() << "transfer to" << comm["thischannel"] << "or" << comm["peerchannel"];
                        emit actionCall("atxfer",
                                        "chan:special:me:" + comm["thischannel"].toString(),
                                        "ext:" + m_ui->phonenumber() );
                        return;
                    }
                }
            }
        }
        // "I" have no current communications, just call the person...
        dial();
    }
}

