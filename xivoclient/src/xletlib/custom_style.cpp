
#include <QMargins>
#include <QtWidgets>
#include <QDebug>
#include <QStyle>
#include <QSize>

#include "custom_style.h"

CustomStyle::CustomStyle()
{
}


QRect CustomStyle::subElementRect(SubElement element,
                                  const QStyleOption * option,
                                  const QWidget * widget) const
{
    if (element == QStyle::SE_ItemViewItemDecoration){
       // qDebug() << element << " " << QStyle::SE_ItemViewItemText;
        QRect text_rect = QProxyStyle::subElementRect(element, option, widget).marginsRemoved(QMargins(30,0,0,0));
       // qDebug() << text_rect;
        return QRect();
    }
    return QProxyStyle::subElementRect(element, option, widget);
}
