#ifndef OUTLOOK_DB_H_INCLUDED
#define OUTLOOK_DB_H_INCLUDED

#ifdef USE_OUTLOOK

#include "sqlite3/sqlite3.h"

#include "outlook_contact.h"

#include <QString>

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
	operator sqlite3 *()const{return m_pDB;}

private:
	bool create_contact(COLContact & contact, const COLPropsDef & props_def);
	bool delete_contact(COLContact & contact);
	bool update_contact(COLContact & contact, const COLPropsDef & props_def);
	bool update_contact_props(COLContact & contact, const COLPropsDef & props_def);


public:
	sqlite3 * m_pDB;
};

class COLQuery {
public:
	COLQuery(sqlite3 * pDB);
	virtual ~COLQuery();

	bool run(const QString & strQuery);

	bool has_row(){return m_bHasRow;}
	bool fetch();

	QString value_str(int col);
	__int64 value_64(int col);
	bool	value_bool(int col);
	double  value_d(int col);

	__int64 last_id();

	QString safe_string(const QString & str);
private:
	void cleanup();

private:
	sqlite3 * m_pDB;
	sqlite3_stmt * m_pStmt;
	bool	m_bHasRow;
	int		m_nColCount;
};

#endif /* USE_OUTLOOK */

#endif /* OUTLOOK_DB_H_INCLUDED */

