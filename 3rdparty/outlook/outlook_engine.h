#ifndef OUTLOOK_ENGINE_H_INCLUDED
#define OUTLOOK_ENGINE_H_INCLUDED

#ifdef USE_OUTLOOK

#include "outlook_thread.h"

class COLEngine {
public:
	COLEngine();
	virtual ~COLEngine();

	void init();

	void start_thread();
	void stop_thread();

	void get_contacts(COLContacts & contacts);
	void get_props_def(COLPropsDef & props_def);
	bool find_contact_by_num(const QString & num, COLContact & contact);

	void update_contacts(COLContacts & contacts);

private:
	int find_contact_by_entry_id(const COLContact & contact);

public:
	COLThread		m_OLThread;

private:
	COLPropsDef	m_props_def;
	COLContacts			m_contacts;
	COLDB				m_db;
	QMutex				m_mutex;
};

COLEngine * OLEngine();
void OLEngineEnd();

#endif

#endif /* OUTLOOK_ENGINE_H_INCLUDED */

