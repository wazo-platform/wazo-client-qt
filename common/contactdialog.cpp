
#include <QFormLayout>
#include <QDialogButtonBox>
#include "contactdialog.h"

ContactDialog::ContactDialog(QWidget * parent)
    : QDialog(parent)
{
    setWindowTitle( tr("Contact") );
    QFormLayout * layout = new QFormLayout( this );
    m_firstname = new QLineEdit();
    layout->addRow( tr("First Name"), m_firstname);
    m_lastname = new QLineEdit();
    layout->addRow( tr("Last Name"), m_lastname);
    m_number = new QLineEdit();
    layout->addRow( tr("Phone Number"), m_number);
    m_company = new QLineEdit();
    layout->addRow( tr("Company"), m_company);
    QDialogButtonBox * buttonBox
        = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addRow( buttonBox );
}

ContactDialog::~ContactDialog()
{
}

