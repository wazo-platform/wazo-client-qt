#ifndef _DSTORE_PRIVATE__
#define _DSTORE_PRIVATE__

#include "dstore.h"

struct ParserRet {
    DStore *origin;
    DStore *ret;
    QString req;
    int abort;
    int count;
};

// INTERNAL (structure to manage callback on QObject)
class DStoreCallback
{
    public:
        DStoreCallback(QObject *on, const char *slot);
        ~DStoreCallback();
        void call(const QString &path, DStoreEvent event);
        QObject* on() { return m_on; }

    private:
        QObject *m_on;
        char *m_slot;
};

#endif
