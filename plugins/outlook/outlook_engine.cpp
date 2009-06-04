#include "outlook_engine.h"
#include "outlook_tools.h"

static COLEngine * gpOLEngine=NULL;

COLEngine * OLEngine() {
	if ( !gpOLEngine ) {
		gpOLEngine=new COLEngine;
	}
	return gpOLEngine;
}

void OLEngineEnd() {
	if ( gpOLEngine ) {
		gpOLEngine->stop_thread();
		delete gpOLEngine;
		gpOLEngine=NULL;
	}
}

COLEngine::COLEngine() {
	init();
}

COLEngine::~COLEngine() {
}

void COLEngine::init() {
	QMutexLocker lock(&m_mutex);	
	//m_db.open();
	//m_db.load_props_def(m_props_def);
	//m_db.load_contacts(m_contacts);
	//m_db.close();
}

void COLEngine::start_thread() {
		gpOLEngine->m_OLThread.start();
}

void COLEngine::stop_thread() {
		gpOLEngine->m_OLThread.m_bStop=true;
		gpOLEngine->m_OLThread.wait();
}

void COLEngine::get_contacts(COLContacts & contacts) {
	QMutexLocker lock(&m_mutex);	
	contacts=m_contacts;
}

void COLEngine::get_props_def(COLPropsDef & props_def) {
	QMutexLocker lock(&m_mutex);	
	props_def=m_props_def;
}

bool COLEngine::find_contact_by_num(const QString & num, COLContact & contact) {
	QMutexLocker lock(&m_mutex);

	QString strNum=cleanup_num(num, true);
	for ( int i = 0, c = m_contacts.count() ; i < c ; i++ ) {
		QHash<QString,QString>::iterator j;
		for ( j = contact.m_properties.begin() ; j != contact.m_properties.end() ; ++j ) {
			QString val=cleanup_num(j.value(), true);
			if ( val == strNum ) {
				contact=m_contacts[i];
				return true;
			}
		}
	}
	return false;
}

int COLEngine::find_contact_by_entry_id(const COLContact & contact) {
	QString strEntryID=contact.m_properties.value("EntryID");
	for ( int i = 0, c = m_contacts.count() ; i < c ; i++ ) {
		if ( m_contacts[i].m_properties.value("EntryID") == strEntryID )
			return i;
	}
	return -1;
}

void COLEngine::update_contacts(COLContacts & contacts) {
	QMutexLocker lock(&m_mutex);
	for ( int i = 0, c = m_contacts.count() ; i < c ; i++ ) {
		m_contacts[i].m_state=OL_STATE_DELETE;
	}

	for ( int i = 0, c = contacts.count() ; i < c ; i++ ) {
		int nIndex=find_contact_by_entry_id(contacts[i]);
		if ( nIndex == -1 ) {
			contacts[i].m_state=OL_STATE_CREATE;
			m_contacts.append(contacts[i]);
		}
		else {
			ASSERTCONTINUE(m_contacts[nIndex].m_state==OL_STATE_DELETE);
			if ( contacts[i].is_newer(m_contacts[nIndex]) ) {
				contacts[i].m_nID=m_contacts[nIndex].m_nID;
				contacts[i].m_state=OL_STATE_UPDATE;
				m_contacts[nIndex]=contacts[i];
			}
			else
				m_contacts[nIndex].m_state=OL_STATE_NO_CHG;
		}
	}
	/*
	m_db.open();

	COLQuery query(m_db);
	if ( !query.run("BEGIN"))
		query.run("ROLLBACK");

	if ( !m_db.update_contacts(m_contacts, m_props_def))
		query.run("ROLLBACK");
	else
		query.run("COMMIT");
	{
	}

	m_db.close();
	*/
}


