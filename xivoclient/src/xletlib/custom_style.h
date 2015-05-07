
#ifndef __CUSTOM_STYLE_H__
#define __CUSTOM_STYLE_H__

#include <QProxyStyle>
//#include <QCommonStyle>
#include <QRect>
#include <QStyleOption>
#include <QWidget>

class CustomStyle : public QProxyStyle
{
    Q_OBJECT

    public:
        CustomStyle();
        ~CustomStyle() {}

        virtual QRect subElementRect(SubElement element,
                                     const QStyleOption * option,
                                     const QWidget * widget = 0) const;
};

#endif
