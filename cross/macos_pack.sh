#!/bin/bash

QTPATH=/opt/qt-4.6.3-shared
cd ./bin/

sed -i "" -e "s/Created by Qt\/QMake/Created by Avencall/g" \
    -e "s/com\.yourcompany\.xivoclient/com.proformatique.xivoclient/g" \
    -e "s/appli\.rc/xivo.icns/g" ./xivoclient.app/Contents/Info.plist

sips -s format icns ../images/xivoicon.ico --out xivoclient.app/Contents/Resources/xivo.icns
rm ./xivoclient.app/Contents/Resources/appli.rc

echo "-- macdeployqt step"
macdeployqt xivoclient.app

echo "-- install_name_tool steps"
install_name_tool -change @executable_path/../Frameworks/libxivoclientbaselib.1.dylib \
    @executable_path/libxivoclientbaselib.1.dylib \
    ./xivoclient.app/Contents/MacOS/xivoclient

for mylib in ./libxivoclientbaselib.dylib ./xivoclient.app/Contents/Resources/plugins/*.dylib ; do
    echo "Qt transformations : ${mylib}"
    install_name_tool -change ${QTPATH}/lib/QtGui.framework/Versions/4/QtGui \
        @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui ${mylib}
    install_name_tool -change ${QTPATH}/lib/QtNetwork.framework/Versions/4/QtNetwork \
        @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork ${mylib}
    install_name_tool -change ${QTPATH}/lib/QtCore.framework/Versions/4/QtCore \
        @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore ${mylib}
done

for mylib in ./xivoclient.app/Contents/Resources/plugins/*.dylib ; do
    echo "libxivoclientbaselib transformations : ${mylib}"
    install_name_tool -change libxivoclientbaselib.1.dylib \
        @executable_path/libxivoclientbaselib.1.dylib ${mylib}
done

cp -R *dylib ./xivoclient.app/Contents/MacOS

echo "-- hdiutil step"
hdiutil create xivoclient.dmg -srcfolder xivoclient.app -format UDZO
