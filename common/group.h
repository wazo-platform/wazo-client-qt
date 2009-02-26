#ifndef __GROUP_H__
#define __GROUP_H__
#include <QObject>
#include <QRect>
#include <QString>
#include <QColor>

class Group : public QObject
{
public:
    Group(QObject * parent = 0);
    void setName(const QString & name) { m_name = name; };
    const QString & name() const { return m_name; };
    void setRect(const QRect & rect) { m_rect = rect; };
    const QRect & rect() const { return m_rect; };
    QRect & rect() { return m_rect; };
    void setColor(const QColor & color) { m_color = color; };
    const QColor & color() const { return m_color; };
private:
    QString m_name;
    QRect m_rect;
    QColor m_color;
};

#endif
