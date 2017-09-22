include(../../../common-xlets.pri)

FORMS += agent_status.ui
QT += widgets

HEADERS = *.h
SOURCES = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/agent_status_dashboard_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agent_status_dashboard_es_CL.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agent_status_dashboard_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agent_status_dashboard_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agent_status_dashboard_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agent_status_dashboard_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agent_status_dashboard_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agent_status_dashboard_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agent_status_dashboard_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agent_status_dashboard_nl.ts

TARGET      = agentstatusdashboardplugin

RESOURCES = agent_status_dashboard.qrc
