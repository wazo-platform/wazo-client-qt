# Short reminder for configure options when building a new Qt static version
# $Date$
# $Revision$

* Linux
-------
./configure \
-static -release -fast -no-qt3support \
-no-phonon -no-webkit -no-sql-sqlite \
-prefix ${PWD}


* Win32
-------
configure.exe \
-static -release -fast -no-qt3support \
-no-phonon -no-webkit -no-sql-sqlite \
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
-universal \
-prefix ${PWD}

