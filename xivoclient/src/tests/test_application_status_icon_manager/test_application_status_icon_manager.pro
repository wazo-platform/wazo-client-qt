include (../../../common-tests.pri)

TARGET = test_application_status_icon_manager

TEST_DIR = $${ROOT_DIR}/src/tests/test_application_status_icon_manager
TESTED_DIR = $${ROOT_DIR}/src

INCLUDEPATH += $${TESTED_DIR}
INCLUDEPATH += $${MOC_DIR}

SOURCES += $${TEST_DIR}/*.cpp
HEADERS += $${TESTED_DIR}/application_status_icon_manager.h
SOURCES += $${TESTED_DIR}/application_status_icon_manager.cpp

RESOURCES += images.qrc
