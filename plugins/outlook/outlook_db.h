#ifndef OUTLOOK_DB_H_INCLUDED
#define OUTLOOK_DB_H_INCLUDED

#include "outlook_contact.h"

#include <QString>
#include <QtSql>

class COLDB {
public:
	COLDB();
	virtual ~COLDB();

	// open/close
	bool open();
	void close();

	// create or update schema
	bool create();

	// data
	bool load_props_def(COLPropsDef & props_def);
	bool load_contacts(COLContacts & contacts);
	bool update_contacts(COLContacts & contacts, const COLPropsDef & props_def);

	// misc
	bool table_exist(const QString & strTableName);
	//operator sqlite3 *()const{return m_pDB;}

private:
	bool create_contact(COLContact & contact, const COLPropsDef & props_def);
	bool delete_contact(COLContact & contact);
	bool update_contact(COLContact & contact, const COLPropsDef & props_def);
	bool update_contact_props(COLContact & contact, const COLPropsDef & props_def);

public:
	QSqlDatabase m_db;	// Database object
};

#endif /* OUTLOOK_DB_H_INCLUDED */
