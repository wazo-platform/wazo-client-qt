#ifndef OUTLOOK_THREAD_H_INCLUDED
#define OUTLOOK_THREAD_H_INCLUDED

#include <QThread>
#include <QMutex>

#include "outlook_db.h"

// refresh interval for address book (in ms)
#define OL_REFRESH_INTERVAL (5 * 60 * 1000)

class COLThread : public QThread {
        Q_OBJECT
    public:
	COLThread();
	virtual ~COLThread();

    protected:
        void run();

    signals:
        void contactsLoaded();
        void errorMessage(const QString &);
        void logClient(const QString &, const QString &, const QString &);

    private:
        bool load_contacts_from_outlook(COLContacts & contacts);
        void update_contacts(COLContacts & contacts);

    public:
	bool m_bStop;
};

#endif /* OUTLOOK_THREAD_H_INCLUDED */
