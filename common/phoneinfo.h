/* $Revision$
 * $Date$
 */
#ifndef __PHONEINFO_H__
#define __PHONEINFO_H__

#include <QString>
#include <QVariant>
#include <QMap>

/*! \brief Store Phone information
 */
class PhoneInfo
{
public:
    //! constructor
    PhoneInfo(const QString & astid,
              const QMap<QString, QVariant> & prop);
    //! destructor
    ~PhoneInfo();
    //! update attribute members
    void update(const QMap<QString, QVariant> & prop);
    //! context this phone belongs to
    const QString & context() const { return m_context; };
    //! access to the status values of this phone
    const QString hintstatus(const QString & key) const {
        return m_hintstatus.contains(key) ? m_hintstatus.value(key) : QString("");
    };
    //! phone number
    const QString & number() const { return m_number; };
    //! phone technology (sip, iax, etc...)
    const QString & tech() const { return m_tech; };
    //! phone id
    const QString & phoneid() const { return m_phoneid; };
    //! current communications of this phone
    const QMap<QString, QVariant> & comms() const { return m_comms; };
private:
    QString m_astid;
    QString m_tech;
    QString m_context;
    QString m_phoneid;
    QString m_number;
    bool m_initialized;
    bool m_enable_hint;
    QMap<QString, QString> m_hintstatus;
    QMap<QString, QVariant> m_comms;
    
};

#endif

