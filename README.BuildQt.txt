# Short reminder for configure options when building a new Qt static version
# List every option : ./configure -help

* Linux
-------
./configure \
-release -fast -opensource \
-no-qt3support \
-no-multimedia \
-no-phonon \
-no-webkit \
-no-javascript-jit \
-no-script \
-no-scripttools \
-no-declarative \
-no-declarative-debug \
-no-cups \
-no-opengl \
-prefix ${PWD}

make sub-src sub-translations

** These are not necessary (disabled by default)
-no-sql-sqlite \
-no-nas-sound \

** Do not use (create .a libs, instead of .so)
-static

** Ubuntu packages prerequisites to build Qt

libx11-dev libxau-dev libxaw7-dev libxcb1-dev libxcomposite-dev libxcursor-dev libxdamage-dev libxdmcp-dev libxext-dev libxfixes-dev libxfont-dev libxft-dev libxi-dev libxinerama-dev libxkbfile-dev libxmu-dev libxmuu-dev libxpm-dev libxrandr-dev libxrender-dev libxres-dev libxss-dev libxt-dev libxtst-dev libxv-dev libxvmc-dev


* Win32
-------
configure.exe \
-release -fast -opensource \
-no-qt3support \
-no-multimedia \
-no-phonon \
-no-webkit \
-no-javascript-jit \
-no-script \
-no-scripttools \
-no-declarative \
-no-declarative-debug \
-no-cups \
-no-opengl \
-openssl-linked \
-L C:/${OPENSSL_PATH}/openssl-0.9.8g \
-I C:/${OPENSSL_PATH}/openssl-0.9.8g/include \
OPENSSL_LIBS= "OPENSSL_LIBS=-lcrypto -lssl" \
-prefix ${PWD}

make sub-src sub-translations

* MacOS
-------
./configure \
-release -fast -opensource \
-no-qt3support \
-no-multimedia \
-no-phonon \
-no-webkit \
-no-javascript-jit \
-no-script \
-no-scripttools \
-no-declarative \
-no-declarative-debug \
-no-cups \
-no-opengl \
-universal \
-prefix ${PWD}

make sub-src sub-translations
