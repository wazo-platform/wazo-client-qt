#ifndef __BASELIB_EXPORT_H__
#define __BASELIB_EXPORT_H__
#include <QtGlobal>

#if defined(BASELIB_LIBRARY)
#define BASELIB_EXPORT Q_DECL_EXPORT
#else
#define BASELIB_EXPORT Q_DECL_IMPORT
#endif

#endif

