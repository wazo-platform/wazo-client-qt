#include "phoneinfo.h"

PhoneInfo::PhoneInfo(const QString & astid,
                     const QMap<QString, QVariant> & prop)
    : m_astid(astid), m_initialized(false), m_enable_hint(false)
{
    m_tech = prop["tech"].toString();
    m_context = prop["context"].toString();
    m_phoneid = prop["phoneid"].toString();
    m_number = prop["number"].toString();
    update(prop);
}

PhoneInfo::~PhoneInfo()
{
}

void PhoneInfo::update(const QMap<QString, QVariant> & prop)
{
    if(prop.contains("initialized"))
        m_initialized = prop["initialized"].toBool();
    if(prop.contains("enable_hint"))
        m_enable_hint = prop["enable_hint"].toBool();
    if(prop.contains("hintstatus"))
    {
        m_hintstatus.clear();
        QMap<QString, QVariant> hintstatus = prop["hintstatus"].toMap();
        foreach(const QString key, hintstatus.keys())
        {
            m_hintstatus[key] = hintstatus[key].toString();
        }
    }
    if(prop.contains("comms"))
    {
        m_comms = prop["comms"].toMap();
    }
}
