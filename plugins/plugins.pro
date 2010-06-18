# build projects contained in subdirectories
TEMPLATE = subdirs
SUBDIRS  = xletweb videoxlet xletnull
QT += network
win32:SUBDIRS += outlook

