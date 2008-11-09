#ifdef USE_OUTLOOK

#include <windows.h>
#include <QString>
#include <QDebug>

#include "outlook_db.h"
#include "outlook_tools.h"


/* === taken from res_sqlite */

#define RES_SQLITE_MAX_LOOPS 10

#define RES_SQLITE_BEGIN                                        \
  do {                                                          \
  int __i;                                                      \
                                                                \
  for (__i = 0; __i < RES_SQLITE_MAX_LOOPS; __i++)              \
    {															\
	do {}while(0)

#define RES_SQLITE_END(error)                                   \
      if ((error != SQLITE_BUSY) && (error != SQLITE_LOCKED))   \
        break;                                                  \
      Sleep(10);                                             \
    }                                                           \
  }while (0)

/* === end of taken from res_sqlite */


// we store outlook addresses in
// <user's directory>\Application Data\Proformatique\xivoclient\outlook.db

static QString get_local_db() {
	static QString strPath;
	if ( strPath.isEmpty() ) {
		// <user's directory>\Application Data
		strPath=getenv("APPDATA");

		strPath+="\\Proformatique";
		if ( !OLPathExist(strPath) && !OLCreateDirectory(strPath) ) {
			// bla bla bla
			strPath="";
			return "";
		}

		strPath+="\\xivoclient";
		if ( !OLPathExist(strPath) && !OLCreateDirectory(strPath) ) {
			// bla bla bla
			strPath="";
			return "";
		}

		strPath+="\\outlook.db";
	}
	return strPath;
}

COLDB::COLDB() {
	m_pDB=NULL;
}

COLDB::~COLDB() {
	close();
}

bool COLDB::open() {
	close();

	if ( SQLITE_OK != sqlite3_open(get_local_db().toAscii(), &m_pDB) ) {
		// bla bla bla
		return false;
	}
	return create();
}

bool COLDB::table_exist(const QString & strTableName) {
	COLQuery query(*this);
	QString strQuery="SELECT count(*) FROM ";
	strQuery+=strTableName;

	return query.run(strQuery);
}

void COLDB::close() {
	if ( m_pDB ) {
		if ( SQLITE_OK != sqlite3_close(m_pDB) ) {
			// bla bla bla
		}
		m_pDB=NULL;
	}
}

bool COLDB::create() {
	COLQuery query(*this);

	if ( !table_exist("props_def") ) {
		if ( !query.run("CREATE TABLE props_def(\
	PROP_ID INTEGER PRIMARY KEY,\
	NAME TEXT NOT NULL DEFAULT '',\
	DISPLAY_NAME TEXT NOT NULL DEFAULT '',\
	DISPLAYABLE INT(1) NOT NULL DEFAULT 0)") ) {
			// bla bla bla
			return false;
		}

		if ( !query.run("INSERT INTO props_def(name, display_name, displayable) values('FirstName', 'First Name', 0)")) { /* bla bla bla */ return false;}
		if ( !query.run("INSERT INTO props_def(name, display_name, displayable) values('LastName', 'Last Name', 0)")) { /* bla bla bla */ return false;}
		if ( !query.run("INSERT INTO props_def(name, display_name, displayable) values('FullName', 'Full Name', 1)")) { /* bla bla bla */ return false;}
		if ( !query.run("INSERT INTO props_def(name, display_name, displayable) values('BusinessTelephoneNumber', 'Business Phone', 1)")) { /* bla bla bla */ return false;}
		if ( !query.run("INSERT INTO props_def(name, display_name, displayable) values('MobileTelephoneNumber', 'Mobile Phone', 1)")) { /* bla bla bla */ return false;}
		if ( !query.run("INSERT INTO props_def(name, display_name, displayable) values('HomeTelephoneNumber', 'Home Phone', 1)")) { /* bla bla bla */ return false;}
		if ( !query.run("INSERT INTO props_def(name, display_name, displayable) values('Email1Address', 'Email Address', 1)")) { /* bla bla bla */ return false;}
		if ( !query.run("INSERT INTO props_def(name, display_name, displayable) values('EntryID', 'EntryID', 0)")) { /* bla bla bla */ return false;}
	}

	if ( !table_exist("folders") ) {
		if ( !query.run("CREATE TABLE folders(\
	ID INTEGER PRIMARY KEY)") ) {
			// bla bla bla
			return false;
		}

		if ( !query.run("INSERT INTO folders(id) values(10)")) { /* bla bla bla */ return false;}
	}

	if ( !table_exist("contacts") ) {
		if ( !query.run("CREATE TABLE contacts(\
	CONTACT_ID INTEGER PRIMARY KEY, \
	FOLDER_ID INTEGER NOT NULL DEFAULT 0, \
	CREATED REAL NOT NULL DEFAULT 0, \
	LAST_MODIFIED REAL NOT NULL DEFAULT 0)") ) {
			// bla bla bla
			return false;
		}
	}


	if ( !table_exist("props_vals") ) {
		if ( !query.run("CREATE TABLE props_vals(\
	CONTACT_ID INTEGER NOT NULL DEFAULT 0, \
	PROP_ID INTEGER NOT NULL DEFAULT 0, \
	PROP_VALUE TEXT NOT NULL DEFAULT '')") ) {
			// bla bla bla
			return false;
		}
	}

	return true;
}

bool COLDB::load_props_def(COLPropsDef & props_def) {

	COLQuery query(*this);

	if ( !query.run("SELECT prop_id, name, display_name, displayable FROM props_def") ) {
		// bla bla bla
		return FALSE;
	}

	for ( ; query.has_row() ; query.fetch() ) {
		props_def.insert(
			query.value_str(1),
			COLPropDef(
				query.value_64(0),
				query.value_str(1),
				query.value_str(2),
				query.value_bool(3)));
	}

	return true;
}

bool COLDB::load_contacts(COLContacts & contacts) {
	COLQuery query(*this);

	if ( !query.run("SELECT contact_id, folder_id, created, last_modified from contacts")) {
		// bla bla bla
		return false;
	}

	COLContact contact;
	for ( ; query.has_row() ; query.fetch() ) {
		contacts.append(contact);
		COLContact & c=contacts.last();
		c.m_properties.clear();
		c.m_nID=query.value_64(0);
		c.m_nFolderID=query.value_64(1);
		c.m_dtCreate=query.value_d(2);
		c.m_dtLastModified=query.value_d(3);

		COLQuery query2(*this);
		QString strQuery=QString("select a.name, b.prop_value from props_def a, props_vals b where a.prop_id=b.prop_id and b.contact_id=")+QString::number(c.m_nID);
		if ( !query2.run(strQuery) ) {
			// bla bla bla
			return FALSE;
		}

		for ( ; query2.has_row() ; query2.fetch() ) {
			c.m_properties.insert(query2.value_str(0), query2.value_str(1));
		}
	}

	return true;
}

bool COLDB::create_contact(COLContact & contact, const COLPropsDef & props_def) {

	sqlite3_stmt * stmt;

	QString strQuery="insert into contacts(created, last_modified) values (?1,?2)";
	int error=sqlite3_prepare_v2(m_pDB, strQuery.toUtf8(), -1, &stmt, NULL);
	if ( error != SQLITE_OK ) {
		// bla bla bla
		qDebug() << "sqlite3_prepare_v2: " << strQuery;
		qDebug() << sqlite3_errmsg(m_pDB);
		return false;
	}

	sqlite3_bind_double(stmt, 1, contact.m_dtCreate);
	sqlite3_bind_double(stmt, 2, contact.m_dtLastModified);

	error=sqlite3_step(stmt);
	if ( (error != SQLITE_OK) && (error != SQLITE_DONE) && (error != SQLITE_ROW) ) {
		// bla bla bla
		qDebug() << "sqlite3_prepare_v2: " << strQuery;
		qDebug() << sqlite3_errmsg(m_pDB);
		sqlite3_finalize(stmt);
		return false;
	}

	contact.m_nID=sqlite3_last_insert_rowid(m_pDB);

	sqlite3_finalize(stmt);

	COLPropsDef::const_iterator i;


	strQuery="insert into props_vals(contact_id, prop_id, prop_value) values (?1,?2,?3)";
	error=sqlite3_prepare_v2(m_pDB, strQuery.toUtf8(), -1, &stmt, NULL);
	if ( error != SQLITE_OK ) {
		// bla bla bla
		qDebug() << "sqlite3_prepare_v2: " << strQuery;
		qDebug() << sqlite3_errmsg(m_pDB);
		return false;
	}

	for ( i = props_def.begin() ; i != props_def.end() ; ++i ) {
		sqlite3_bind_int64(stmt, 1, contact.m_nID);
		sqlite3_bind_int64(stmt, 2, i.value().m_nID);
		sqlite3_bind_text(stmt, 3, contact.m_properties.value(i.key()).toAscii(), -1, SQLITE_TRANSIENT);

		error=sqlite3_step(stmt);
		if ( (error != SQLITE_OK) && (error != SQLITE_DONE) && (error != SQLITE_ROW) ) {
			// bla bla bla
			qDebug() << "sqlite3_prepare_v2: " << strQuery;
			qDebug() << sqlite3_errmsg(m_pDB);
			sqlite3_finalize(stmt);
			return false;
		}
		sqlite3_reset(stmt);
	}

	sqlite3_finalize(stmt);

	return true;
}


bool COLDB::delete_contact(COLContact & contact) {
	COLQuery query(*this);

	QString strQuery="delete from contacts where contact_id=";
	strQuery+=QString::number(contact.m_nID);

	if ( !query.run(strQuery) ) {
		return false;
	}

	strQuery="delete from props_vals where contact_id=";
	strQuery+=QString::number(contact.m_nID);

	if ( !query.run(strQuery) ) {
		return false;
	}

	return true;
}

bool COLDB::update_contact_props(COLContact & contact, const COLPropsDef & props_def) {
	int error;
	COLQuery query(*this);
	sqlite3_stmt * stmt;

	// we delete existing properties
	QString strQuery="delete from props_vals where contact_id=";
	strQuery+=QString::number(contact.m_nID);

	if ( !query.run(strQuery) ) {
		return false;
	}


	// and we insert new ones
	COLPropsDef::const_iterator i;
	strQuery="insert into props_vals(contact_id, prop_id, prop_value) values (?1,?2,?3)";
	error=sqlite3_prepare_v2(m_pDB, strQuery.toUtf8(), -1, &stmt, NULL);
	if ( error != SQLITE_OK ) {
		// bla bla bla
		qDebug() << "sqlite3_prepare_v2: " << strQuery;
		qDebug() << sqlite3_errmsg(m_pDB);
		return false;
	}

	for ( i = props_def.begin() ; i != props_def.end() ; ++i ) {
		sqlite3_bind_int64(stmt, 1, contact.m_nID);
		sqlite3_bind_int64(stmt, 2, i.value().m_nID);
		sqlite3_bind_text(stmt, 3, contact.m_properties.value(i.key()).toAscii(), -1, SQLITE_TRANSIENT);

		error=sqlite3_step(stmt);
		if ( (error != SQLITE_OK) && (error != SQLITE_DONE) && (error != SQLITE_ROW) ) {
			// bla bla bla
			qDebug() << "sqlite3_prepare_v2: " << strQuery;
			qDebug() << sqlite3_errmsg(m_pDB);
			sqlite3_finalize(stmt);
			return false;
		}
		sqlite3_reset(stmt);
	}

	sqlite3_finalize(stmt);

	return true;
}

bool COLDB::update_contact(COLContact & contact, const COLPropsDef & props_def) {

	// we update properties
	if ( !update_contact_props(contact, props_def) )
		return false;

	// we update create/modification time

	sqlite3_stmt * stmt;
	QString strQuery="update contacts set created=?1, last_modified=?2 where contact_id=?3";
	int error=sqlite3_prepare_v2(m_pDB, strQuery.toUtf8(), -1, &stmt, NULL);
	if ( error != SQLITE_OK ) {
		// bla bla bla
		qDebug() << "sqlite3_prepare_v2: " << strQuery;
		qDebug() << sqlite3_errmsg(m_pDB);
		return false;
	}

	sqlite3_bind_double(stmt, 1, contact.m_dtCreate);
	sqlite3_bind_double(stmt, 2, contact.m_dtLastModified);
	sqlite3_bind_int64(stmt, 3, contact.m_nID);

	error=sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	if ( (error != SQLITE_OK) && (error != SQLITE_DONE) && (error != SQLITE_ROW) ) {
		// bla bla bla
		qDebug() << "sqlite3_prepare_v2: " << strQuery;
		qDebug() << sqlite3_errmsg(m_pDB);
		return false;
	}

	return true;
}


bool COLDB::update_contacts(COLContacts & contacts, const COLPropsDef & props_def) {
	for ( int i = 0, c = contacts.count() ; i < c ; i++ ) {
		COLContact & contact=contacts[i];

		switch(contact.m_state) {
			case OL_STATE_CREATE:
				if ( !create_contact(contact, props_def)) return false;
				break;

			case OL_STATE_UPDATE:
				if ( !delete_contact(contact)) return false;
				if ( !create_contact(contact, props_def)) return false;
				break;

			case OL_STATE_DELETE:
				qDebug() << contact.m_properties.value("FullName");
				if ( !delete_contact(contact)) return false;
				contacts.removeAt(i);
				i--;
				c--;
				break;

			case OL_STATE_NO_CHG:
				break;

			default:
				ASSERTCONTINUE(false);
		};

	}
	return true;
}


COLQuery::COLQuery(sqlite3 * pDB) {
	m_pStmt=NULL;
	m_pDB=pDB;
	m_bHasRow=false;
	m_nColCount=0;
}

COLQuery::~COLQuery() {
	cleanup();
}

void COLQuery::cleanup() {
	if ( m_pStmt ) {
		sqlite3_finalize(m_pStmt);
		m_pStmt=NULL;
		m_bHasRow=false;
		m_nColCount=0;
	}
}

bool COLQuery::run(const QString & strQuery) {
	ASSERTRETV(m_pDB, FALSE);

	cleanup();

	int error=sqlite3_prepare_v2(m_pDB, strQuery.toUtf8(), -1, &m_pStmt, NULL);
	if ( error != SQLITE_OK ) {
		// bla bla bla
		qDebug() << "sqlite3_prepare_v2: " << strQuery;
		qDebug() << sqlite3_errmsg(m_pDB);
		return false;
	}

	return fetch();
}

bool COLQuery::fetch() {
	ASSERTRETV(m_pStmt, false);

	int error;

	RES_SQLITE_BEGIN;
	error=sqlite3_step(m_pStmt);
	RES_SQLITE_END(error);
	if ( (error != SQLITE_OK) && (error != SQLITE_DONE) && (error != SQLITE_ROW) ) {
		// bla bla bla
		qDebug() << "sqlite3_step";
		return false;
	}

	if ( !m_bHasRow ) {
		m_nColCount=sqlite3_column_count(m_pStmt);
	}

	m_bHasRow = (error == SQLITE_ROW);
	return true;
}

static int gogo=0;
QString COLQuery::value_str(int col) {
	ASSERTRETV(m_pStmt, "");
	ASSERTRETV(m_bHasRow, "");
	ASSERTRETV(col < m_nColCount, "");

	const unsigned char * val=sqlite3_column_text(m_pStmt, col);
	return QString((const char*)val);
}

__int64 COLQuery::value_64(int col) {
	ASSERTRETV(m_pStmt, -1);
	ASSERTRETV(m_bHasRow, -1);
	ASSERTRETV(col < m_nColCount, -1);

	return sqlite3_column_int64(m_pStmt, col);
}

bool	COLQuery::value_bool(int col) {
	ASSERTRETV(m_pStmt, false);
	ASSERTRETV(m_bHasRow, false);
	ASSERTRETV(col < m_nColCount, false);

	return sqlite3_column_int(m_pStmt, col);
}


double	COLQuery::value_d(int col) {
	ASSERTRETV(m_pStmt, false);
	ASSERTRETV(m_bHasRow, false);
	ASSERTRETV(col < m_nColCount, false);

	return sqlite3_column_double(m_pStmt, col);
}

__int64	COLQuery::last_id() {
	ASSERTRETV(m_pDB, 0);

	return sqlite3_last_insert_rowid(m_pDB);
}

QString COLQuery::safe_string(const QString & str) {
	QString result(str);
	result.replace("'", "''");
	return result;
}

#endif
