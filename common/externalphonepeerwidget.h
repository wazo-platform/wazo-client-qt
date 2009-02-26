#ifndef __EXTERNALPHONEPEERWIDGET_H__
#define __EXTERNALPHONEPEERWIDGET_H__

#include <QSize>
#include "basepeerwidget.h"

class QLabel;

/*! \brief Widget used to represent an external phone number
 *
 */
class ExternalPhonePeerWidget : public BasePeerWidget
{
    Q_OBJECT
public:
    ExternalPhonePeerWidget(BaseEngine *, const QVariant &, const QString &, const QString &);
    void setAgentToolTip(const QString &, const QStringList &) { };
    void setAgentState(const QString &) { };
    void updatePresence() { };
    void updatePhonesStates() { };
    void setName(const QString & name);
    void setText(const QString & text);
    QString name() const;
public slots:
    void edit();
protected:
    void paintEvent(QPaintEvent *);
private:
    //QLabel * m_label;
    QString m_text;
};

#endif
