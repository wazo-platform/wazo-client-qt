#ifndef OUTLOOK_TOOLS_H_INCLUDED
#define OUTLOOK_TOOLS_H_INCLUDED

#include <QString>

#include <windows.h>

#ifdef _DEBUG
#include <assert.h>
#endif

QString cleanup_num(QString str, bool clean_prefix=false);
QString callnum(QString str);

WCHAR* wchar_dup(const char * sz, BOOL bSysAlloc=FALSE);

void wchar_to_qstr(const WCHAR * sz, QString & str);

bool OLPathExist(const QString & strPath);
bool OLCreateDirectory(const QString & strPath);

#ifdef _DEBUG
#	define ASSERTRETV(expr, value)          \
	do {                                    \
		assert(expr);                   \
	}while (0) 

#	define ASSERTCONTINUE(expr)             \
	do {                                    \
		assert(expr);                   \
	}while (0) 

#else
// TODO: add logging of failure
#	define ASSERTRETV(expr, value)          \
	do {                                    \
		if (!(expr))                    \
			return value;           \
	}while (0) 

#	define ASSERTCONTINUE(expr)             \
        if ( !(expr) ) continue 
#endif


#endif /* OUTLOOK_TOOLS_H_INCLUDED */
