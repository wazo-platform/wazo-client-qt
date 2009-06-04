/*
 * author : ???
 *
 * Rewritten to use Qt SQL API by Thomas BERNARD
 */
#include <windows.h>
#include <QString>
#include <QDebug>

#include "outlook_db.h"
#include "outlook_tools.h"


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

COLDB::COLDB()
{
    m_db = QSqlDatabase::addDatabase(QString("QSQLITE"));
}

COLDB::~COLDB() {
	close();
}

bool COLDB::open() {
	close();

	m_db.setDatabaseName( get_local_db() );
	if (!m_db.open())
		return false;
	return create();
}

bool COLDB::table_exist(const QString & strTableName) {
	QSqlQuery query(m_db);
	QString strQuery="SELECT count(*) FROM ";
	strQuery+=strTableName;

	return query.exec(strQuery);
}

void COLDB::close() {
	m_db.close();
}

bool COLDB::create() {
	QSqlQuery query(m_db);

	if ( !table_exist("props_def") ) {
		if ( !query.exec("CREATE TABLE props_def(\
	PROP_ID INTEGER PRIMARY KEY,\
	NAME TEXT NOT NULL DEFAULT '',\
	DISPLAY_NAME TEXT NOT NULL DEFAULT '',\
	DISPLAYABLE INT(1) NOT NULL DEFAULT 0)") ) {
			// bla bla bla
			return false;
		}

		if ( !query.exec("INSERT INTO props_def(name, display_name, displayable) values('FirstName', 'First Name', 0)")) { /* bla bla bla */ return false;}
		if ( !query.exec("INSERT INTO props_def(name, display_name, displayable) values('LastName', 'Last Name', 0)")) { /* bla bla bla */ return false;}
		if ( !query.exec("INSERT INTO props_def(name, display_name, displayable) values('FullName', 'Full Name', 1)")) { /* bla bla bla */ return false;}
		if ( !query.exec("INSERT INTO props_def(name, display_name, displayable) values('BusinessTelephoneNumber', 'Business Phone', 1)")) { /* bla bla bla */ return false;}
		if ( !query.exec("INSERT INTO props_def(name, display_name, displayable) values('MobileTelephoneNumber', 'Mobile Phone', 1)")) { /* bla bla bla */ return false;}
		if ( !query.exec("INSERT INTO props_def(name, display_name, displayable) values('HomeTelephoneNumber', 'Home Phone', 1)")) { /* bla bla bla */ return false;}
		if ( !query.exec("INSERT INTO props_def(name, display_name, displayable) values('Email1Address', 'Email Address', 1)")) { /* bla bla bla */ return false;}
		if ( !query.exec("INSERT INTO props_def(name, display_name, displayable) values('EntryID', 'EntryID', 0)")) { /* bla bla bla */ return false;}
	}

	if ( !table_exist("folders") ) {
		if ( !query.exec("CREATE TABLE folders(\
	ID INTEGER PRIMARY KEY)") ) {
			// bla bla bla
			return false;
		}

		if ( !query.exec("INSERT INTO folders(id) values(10)")) { /* bla bla bla */ return false;}
	}

	if ( !table_exist("contacts") ) {
		if ( !query.exec("CREATE TABLE contacts(\
	CONTACT_ID INTEGER PRIMARY KEY, \
	FOLDER_ID INTEGER NOT NULL DEFAULT 0, \
	CREATED REAL NOT NULL DEFAULT 0, \
	LAST_MODIFIED REAL NOT NULL DEFAULT 0)") ) {
			// bla bla bla
			return false;
		}
	}


	if ( !table_exist("props_vals") ) {
		if ( !query.exec("CREATE TABLE props_vals(\
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

	QSqlQuery query(m_db);

	if ( !query.exec("SELECT prop_id, name, display_name, displayable FROM props_def") ) {
		// bla bla bla
		return FALSE;
	}

	while(query.next()) {
		props_def.insert(
			query.value(1).toString(),
			COLPropDef(
				query.value(0).toLongLong(),
				query.value(1).toString(),
				query.value(2).toString(),
				query.value(3).toBool()));
	}

	return true;
}

bool COLDB::load_contacts(COLContacts & contacts) {
	QSqlQuery query(m_db);

	if ( !query.exec("SELECT contact_id, folder_id, created, last_modified from contacts")) {
		// bla bla bla
		return false;
	}

	COLContact contact;
	while( query.next() ) {
		contacts.append(contact);
		COLContact & c=contacts.last();
		c.m_properties.clear();
		c.m_nID=query.value(0).toLongLong();
		c.m_nFolderID=query.value(1).toLongLong();
		c.m_dtCreate=query.value(2).toDouble();
		c.m_dtLastModified=query.value(3).toDouble();

		QSqlQuery query2(m_db);
		QString strQuery=QString("select a.name, b.prop_value from props_def a, props_vals b where a.prop_id=b.prop_id and b.contact_id=")+QString::number(c.m_nID);
		if ( !query2.exec(strQuery) ) {
			// bla bla bla
			return FALSE;
		}

		while( query2.next() ) {
			c.m_properties.insert(query2.value(0).toString(), query2.value(1).toString());
		}
	}

	return true;
}

bool COLDB::create_contact(COLContact & contact, const COLPropsDef & props_def) {
#if 0
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

#endif
	return true;
}


bool COLDB::delete_contact(COLContact & contact) {
	QSqlQuery query(m_db);

	QString strQuery="delete from contacts where contact_id=";
	strQuery+=QString::number(contact.m_nID);

	if ( !query.exec(strQuery) ) {
		return false;
	}

	strQuery="delete from props_vals where contact_id=";
	strQuery+=QString::number(contact.m_nID);

	if ( !query.exec(strQuery) ) {
		return false;
	}

	return true;
}

bool COLDB::update_contact_props(COLContact & contact, const COLPropsDef & props_def) {
	//int error;
	QSqlQuery query(m_db);

	// we delete existing properties
	QString strQuery="delete from props_vals where contact_id=";
	strQuery+=QString::number(contact.m_nID);

	if ( !query.exec(strQuery) ) {
		return false;
	}

	// and we insert new ones
	COLPropsDef::const_iterator i;
	strQuery="insert into props_vals(contact_id, prop_id, prop_value) values (?, ?, ?)";

	for ( i = props_def.begin() ; i != props_def.end() ; ++i ) {
		query.prepare(strQuery);
		query.addBindValue(contact.m_nID);
		query.addBindValue(i.value().m_nID);
		query.addBindValue(contact.m_properties.value(i.key()));
		query.exec();
	}
	return true;
}

bool COLDB::update_contact(COLContact & contact, const COLPropsDef & props_def) {
	// we update properties
	if ( !update_contact_props(contact, props_def) )
		return false;
	// we update create/modification time

	QString strQuery="update contacts set created=?, last_modified=? where contact_id=?";
	QSqlQuery query(m_db);
	query.addBindValue(contact.m_dtCreate);
	query.addBindValue(contact.m_dtLastModified);
	query.addBindValue(contact.m_nID);
	return query.exec();
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

		}

	}
	return true;
}


