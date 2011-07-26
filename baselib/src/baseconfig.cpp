#include "baseconfig.h"

BaseConfig::BaseConfig()
    : QObject(NULL)
{
}

/*!
 * \return the value associated with key
 */
const QVariant BaseConfig::value(const QString &key, ReadMode rm) const
{
    if (rm == Unmasked) {
        return m_qvm[key];
    } else {
        if (m_qvm_mask.keys().contains(key)) {
            return m_qvm_mask[key];
        } else {
            return m_qvm[key];
        }
    }
}

/*!
 * See value().
 */
const QVariant BaseConfig::operator[](const QString &key) const
{
    return value(key);
}

/*!
 * \return the reference to the value, so that you can assign it
 * This modifies the unmasked value.
 */
QVariant & BaseConfig::operator[](const QString &key)
{
    return m_qvm[key];
}

/*!
 * \return the reference to the value, so that you can assign it
 * This modifies the mask value.
 */
QVariant & BaseConfig::mask(const QString &key)
{
    return m_qvm_mask[key];
}

/*!
 * \return a QVariantMap containing all the values below the name parameter.
 * Hierarchic separator is '.'.
 * Example : 
 * example["x.y.z"] = "a";
 * example["x.z"] = "b";
 * example["z"] = "c";
 * example.getSubset("x") will return the QVariantMap :
 * ret["y.z"] = "a";
 * ret["z"] = "b";
 * \param rm choose the set of value read (masked or unmasked).
 */
QVariantMap BaseConfig::getSubSet (const QString &name, ReadMode rm) const
{
    QVariantMap ret;
    foreach (QString key, m_qvm.keys()) {
        if (key.startsWith(name + '.')) {
            QString newKey = key;
            newKey.remove (0, name.length() + 1);
            ret[newKey] = value(key, rm);
        }
    }
    return ret;
}

/*!
 * \return true if the key parameter is masked by a mask value.
 */
bool BaseConfig::isMasked(const QString &key) const
{
    return m_qvm_mask.keys().contains(key);
}

/*!
 * \return the QVariantMap containing all stored keys and values
 * This returns the unmasked values.
 */
QVariantMap BaseConfig::toQVariantMap() const
{
    return m_qvm;
}

/*!
 * Merges extern_qvm into this BaseConfig. All values already here are replaced.
 * Prefix adds one hierarchical level.
 * This modifies unmasked values.
 */
void BaseConfig::merge (const QVariantMap &extern_qvm, QString prefix)
{
    if (!prefix.isEmpty() && !prefix.endsWith(".")) {
        prefix += ".";
    }
    foreach (QString key, extern_qvm.keys()) {
        this->operator[](prefix + key) = extern_qvm[key];
    }
}

/*!
 * See merge().
 * This modifies mask values.
 */
void BaseConfig::mergeMask (const QVariantMap &extern_qvm, QString prefix)
{
    if (!prefix.isEmpty() && !prefix.endsWith(".")) {
        prefix += ".";
    }
    foreach (QString key, extern_qvm.keys()) {
        this->mask(prefix + key) = extern_qvm[key];
    }
}
