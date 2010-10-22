#ifndef OUTLOOK_CONTACT_H_INCLUDED
#define OUTLOOK_CONTACT_H_INCLUDED

#include <QList>
#include <QHash>

#define OL_STATE_INIT	0x00
#define OL_STATE_CREATE	0x01
#define OL_STATE_UPDATE	0x02
#define OL_STATE_DELETE	0x04
#define OL_STATE_NO_CHG	0x08

class COLContact {
 public:
	COLContact(){m_dtCreate=0;m_dtLastModified=0;m_state=OL_STATE_INIT;}
	virtual ~COLContact(){}
	COLContact(const COLContact & c):
		m_properties(c.m_properties),
		m_nID(c.m_nID),
		m_nFolderID(c.m_nFolderID),
		m_dtCreate(c.m_dtCreate),
		m_dtLastModified(c.m_dtLastModified),
		m_state(c.m_state)
                        {
                        }

                COLContact & operator=(const COLContact & c) {
                        m_properties=c.m_properties;
                        m_nID=c.m_nID;
                        m_nFolderID=c.m_nFolderID;
                        m_dtCreate=c.m_dtCreate;
                        m_dtLastModified=c.m_dtLastModified;
                        m_state=c.m_state;

                        return *this;
                }

                bool	operator <(const COLContact & c) const {
                        QString str1=m_properties.value("FullName");
                        QString str2=c.m_properties.value("FullName");
                        return str1<str2;
                }

                bool is_newer(const COLContact & c) const {
                        return (m_dtCreate > c.m_dtCreate ) ||
                                (m_dtLastModified > c.m_dtLastModified);
                }

 public:
                QHash<QString, QString> m_properties;
                __int64	m_nID;
                __int64 m_nFolderID;
                double m_dtCreate;
                double m_dtLastModified;

                char m_state;
};

typedef QList<COLContact>	COLContacts;



class COLPropDef {
 public:
	COLPropDef(){}
	COLPropDef(__int64 nID, const QString & name, bool displayable = false) :
		m_nID(nID),
		m_strName(name),
		m_bDisplayable(displayable)
                        {}
                virtual ~COLPropDef(void) {}
                COLPropDef(const COLPropDef & def) {
                        (*this)=def;
                }

                COLPropDef & operator=(const COLPropDef & def) {
                        m_nID=def.m_nID;
                        m_strName=def.m_strName;
                        m_bDisplayable=def.m_bDisplayable;
                        return *this;
                }

 public:
                __int64 m_nID;
                QString m_strName;
                bool	m_bDisplayable;
};

typedef QHash<QString, COLPropDef>	COLPropsDef;

#endif
