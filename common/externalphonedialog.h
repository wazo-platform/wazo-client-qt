#ifndef __EXTERNALPHONEDIALOG_H__
#define __EXTERNALPHONEDIALOG_H__

#include <QDialog>
#include <QLineEdit>

/*! \brief Dialog used to enter phone number and label
 */
class ExternalPhoneDialog : public QDialog
{
public:
    ExternalPhoneDialog(QWidget * parent = 0);
    ~ExternalPhoneDialog();
    QString label() const { return m_label?m_label->text():QString(); };
    void setLabel(const QString & label) { if(m_label) m_label->setText(label); };
    QString number() const { return m_number?m_number->text():QString(); };
    void setNumber(const QString & number) { if(m_number) m_number->setText(number); };
private:
    QLineEdit * m_label;
    QLineEdit * m_number;
};

#endif

