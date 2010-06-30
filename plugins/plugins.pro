# build projects contained in subdirectories
TEMPLATE = subdirs
SUBDIRS  = xletweb videoxlet xletnull conference2
QT += network
win32:SUBDIRS += outlook

