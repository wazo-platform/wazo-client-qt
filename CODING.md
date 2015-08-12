Implementation traps
====================

QFileDialog::DontUseNativeDialog
--------------------------------

It has been seen on Ubuntu that using QFileDialog without the option
DontUseNativeDialog triggers the signal QFileDialog::fileSelected twice. See
QTBUG-38985. Hence file dialogs should use the DontUseNativeDialog option.


QDialog
-------

Some QDialog should not be used with QDialog::exec(), or they risk a segfault.
When the QDialog showing is due to a GUI action, exec() is fine, but when the
QDialog showing is due to a network response, the QDialog::exec() will block the
network event loop, and no other network messages will be processed while the
QDialog::exec() is running. This might cause a network disconnection due to the
keepalive mechanism, and provoke a segfault if the conditions are right.
