# Short reminder for configure options when building a new Qt static version
# $Date: 2008-11-10 17:23:39 +0100 (Mon, 10 Nov 2008) $
# $Revision: 4637 $

* Linux
-------
./configure \
-static -release -fast -no-qt3support \
-no-phonon -no-webkit -no-sql-sqlite \
-no-nas-sound \
-prefix ${PWD}


* Win32
-------
configure.exe \
-static -release -fast -no-qt3support \
-no-phonon -no-webkit -no-sql-sqlite \
-no-nas-sound \
-openssl-linked \
-L C:/${OPENSSL_PATH}/openssl-0.9.8g \
-I C:/${OPENSSL_PATH}/openssl-0.9.8g/include \
OPENSSL_LIBS= "OPENSSL_LIBS=-lcrypto -lssl" \
-prefix ${PWD}


* MacOS
-------
./configure \
-static -release -fast -no-qt3support \
-no-phonon -no-webkit -no-sql-sqlite \
-no-nas-sound \
-universal \
-prefix ${PWD}

