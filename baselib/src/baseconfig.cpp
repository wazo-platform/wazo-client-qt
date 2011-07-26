#include "baseconfig.h"

BaseConfig::BaseConfig()
    : QObject(NULL)
{
}

/*!
 * \return the value associated with key
 */
const QVariant BaseConfig::operator[](const QString &key) const
{
    return m_qvm[key];
}

/*!
 * \return the reference to the value, so that you can assign it
 * If the wanted value is frozen, the function will succeed but the value won't be affected (the returned reference will be a dummy one).
 */
QVariant & BaseConfig::operator[](const QString &key)
{
    if (m_freezemode) {
        m_frozen << key;
        m_frozen.removeDuplicates();
    }
    if (!m_freezemode && m_frozen.contains(key)) {
        /*! \note Possible "security" hole : a plugin A can know what another B tried to set to the frozen value if A stores the reference and reads it after B tries writing
         */
        m_blackhole = QVariant();
        return m_blackhole;
    } else {
        return m_qvm[key];
    }
}

/*!
 * \brief enable or disable the freeze mode.
 * When enabled, you can write any value. These values are marked as frozen.
 * When disabled, you can't write the frozen values.
 */
void BaseConfig::setFreezeMode(bool mode)
{
    m_freezemode = mode;
}

/*!
 * \return a QVariantMap containing all the values below the name parameter.
 * Example : 
 * example["x.y.z"] = "a";
 * example["x.z"] = "b";
 * example["z"] = "c";
 * example.getSubset("x") will return the QVariantMap :
 * ret["y.z"] = "a";
 * ret["z"] = "b";
 */
QVariantMap BaseConfig::getSubSet (const QString &name)
{
    QVariantMap ret;
    foreach (QString key, m_qvm.keys()) {
        if (key.startsWith(name + '.')) {
            QString newKey = key;
            newKey.remove (0, name.length());
            ret[newKey] = m_qvm[key];
        }
    }
    return ret;
}

/*!
 * \return true if the key parameter is frozen (read-only)
 */
bool BaseConfig::isFrozen(const QString &key)
{
    return m_frozen.contains(key);
}

/*!
 * \return the QVariantMap containing stored keys and values
 */
QVariantMap BaseConfig::toQVariantMap()
{
    return m_qvm;
}

/*!
 * Merges extern_qvm into this BaseConfig. All values with the same keys are replaced (if not frozen).
 */
void BaseConfig::merge (QVariantMap extern_qvm)
{
    foreach (QString key, extern_qvm.keys()) {
        this->operator[](key) = extern_qvm[key];
    }
}
