include (../../../common-tests.pri)

TARGET = test_systray_manager

TEST_DIR = $${ROOT_DIR}/src/tests/test_systray_manager
TESTED_DIR = $${ROOT_DIR}/src

INCLUDEPATH += $${TESTED_DIR}
INCLUDEPATH += $${MOC_DIR}

SOURCES += $${TEST_DIR}/*.cpp
HEADERS += $${TESTED_DIR}/application_status_icon_manager.h
HEADERS += $${TESTED_DIR}/systray_manager.h
SOURCES += $${TESTED_DIR}/systray_manager.cpp
