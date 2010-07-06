#include "dstore.h"

DStoreCallback::DStoreCallback(QObject* _on, const char *_slot)
    : on(_on)
{
    int len = strlen(_slot);
    slot = new char[len];
    memcpy(slot, _slot+1, len);
}

DStoreCallback::~DStoreCallback()
{
    delete[] slot;
}

void DStoreCallback::call(const QString &path, DStoreEvent event)
{
  QMetaObject::invokeMethod(
      on,
      slot,
      Qt::DirectConnection,
      Q_ARG(const QString, path),
      Q_ARG(DStoreEvent, event));
}
