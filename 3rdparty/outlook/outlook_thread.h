#ifndef OUTLOOK_THREAD_H_INCLUDED
#define OUTLOOK_THREAD_H_INCLUDED

#ifdef USE_OUTLOOK

#include <QThread>
#include <QMutex>

#include "outlook_db.h"

// refresh interval for addresse book (in ms)
// one hour for now
#define OL_REFRESH_INTERVAL (2*60*1000)

class COLThread : public QThread {
     Q_OBJECT

public:
	COLThread();
	virtual ~COLThread();

protected:
     void run();

signals:
    void contactsLoaded();

private:
	 bool load_contacts_from_outlook(COLContacts & contacts);
	 void update_contacts(COLContacts & contacts);

public:
	bool m_bStop;
};


#endif /* USE_OUTLOOK */

#endif /* OUTLOOK_THREAD_H_INCLUDED */

