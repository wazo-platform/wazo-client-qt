#include <QDebug>
#include <QFormLayout>
#include <QDialogButtonBox>
#include "externalphonedialog.h"

ExternalPhoneDialog::ExternalPhoneDialog(QWidget * parent)
    : QDialog(parent, 0)
{
    setWindowTitle( tr("External phone number") );
    QFormLayout * layout = new QFormLayout( this );
    m_label = new QLineEdit();
    layout->addRow( tr("Label"), m_label );
    m_number = new QLineEdit();
    layout->addRow( tr("Phone number"), m_number );
    QDialogButtonBox * buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addRow( buttonBox );
}

ExternalPhoneDialog::~ExternalPhoneDialog()
{
    qDebug() << "~ExternalPhoneDialog()";
}

