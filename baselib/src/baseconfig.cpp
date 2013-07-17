/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "baseconfig.h"

BaseConfig::BaseConfig()
    : QObject()
{
}

const QVariant BaseConfig::value(const QString &key) const
{
    return m_qvm[key];
}

const QVariant BaseConfig::operator[](const QString &key) const
{
    return value(key);
}

QVariant & BaseConfig::operator[](const QString &key)
{
    return m_qvm[key];
}

/*!
 * \return a QVariantMap containing all the values hierarchically below the name parameter.
 *
 * Hierarchic separator is '.'.\n
 * Example :
 * \code
 * example["x.y.z"] = "a";
 * example["x.z"] = "b";
 * example["z"] = "c";
 * \endcode
 * example.getSubset("x") will return the QVariantMap :
 * \code
 * ret["y.z"] = "a";
 * ret["z"] = "b";
 * \endcode
 */
QVariantMap BaseConfig::getSubSet (const QString &name) const
{
    QVariantMap ret;
    foreach (QString key, m_qvm.keys()) {
        if (key.startsWith(name + '.')) {
            QString newKey = key;
            newKey.remove (0, name.length() + 1);
            ret[newKey] = value(key);
        }
    }
    return ret;
}

QVariantMap BaseConfig::toQVariantMap() const
{
    return m_qvm;
}

/*!
 * Merges extern_qvm into this BaseConfig.
 *
 * \param prefix prefixes every key. A '.' will be appended to prefix if not present.
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

bool BaseConfig::contains(const QString &key)
{
    return m_qvm.contains(key);
}

QString BaseConfig::toString()
{
    QString ret("\n"), line("%1, %2\n");
    foreach (QString key, m_qvm.keys()) {
        ret += line.arg(key, -50).arg(value(key).canConvert(QVariant::String)
                                      ? value(key).toString()
                                      : value(key).typeName());
    }
    return ret;
}

QStringList BaseConfig::keys()
{
    QStringList ret = m_qvm.keys();
    ret.removeDuplicates();
    return ret;
}

ConnectionConfig BaseConfig::getConnectionConfig()
{
    ConnectionConfig ret = {
        this->value("cti_address").toString(),
        this->value("cti_port").toUInt(),
        this->value("cti_encrypt").toBool(),
        this->value("cti_backup_address").toString(),
        this->value("cti_backup_port").toUInt(),
        this->value("cti_backup_encrypt").toBool()
    };
    return ret;
}
