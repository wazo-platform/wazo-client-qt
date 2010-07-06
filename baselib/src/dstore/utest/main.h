#ifndef __MAIN_HEADER
#define __MAIN_HEADER

#include "dstore.h"


class WatchChange : public QObject {
    Q_OBJECT
    
    public slots:
        void allTreeChange(const QString &path, DStoreEvent event);
        void specificTreeChange(const QString &path, DStoreEvent event);
};

#endif
