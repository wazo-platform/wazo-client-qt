#ifndef __EXTERNALPHONEDIALOG_H__
#define __EXTERNALPHONEDIALOG_H__

#include <QDialog>
#include <QLineEdit>

/*! \brief Dialog used to enter phone number and label
 */
class ExternalPhoneDialog : public QDialog
{
    Q_OBJECT
public:
    ExternalPhoneDialog(QWidget * parent = 0);
    ~ExternalPhoneDialog();
    //! return text label
    QString label() const { return m_label?m_label->text():QString(); };
    //! set text label
    void setLabel(const QString & label) { if(m_label) m_label->setText(label); };
    //! return phone number text
    QString number() const { return m_number?m_number->text():QString(); };
    //! set phone number text
    void setNumber(const QString & number) { if(m_number) m_number->setText(number); };
private:
    //! widget for entering label
    QLineEdit * m_label;
    //! widget for entering phone number
    QLineEdit * m_number;
};

#endif

