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

#endif
