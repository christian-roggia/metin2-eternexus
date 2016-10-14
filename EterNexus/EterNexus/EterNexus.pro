#-------------------------------------------------
#
# Project created by QtCreator 2012-11-15T18:55:53
#
#-------------------------------------------------

QT       += core gui webkit network

TARGET = EterNexus
TEMPLATE = app


SOURCES += main.cpp\
        EterNexus.cpp \
    MultiThread.cpp \
    UI/Settings.cpp \
    Global/Utils.cpp \
    Global/IO.cpp \
    Global/Global.cpp \
    UI/About.cpp \
    Global/Version.cpp \
    Global/log.c \
    ArchivationHandler.cpp \
    ExtractionHandler.cpp \
    Language.cpp

HEADERS  += EterNexus.h \
    API/EterPackAPI.hpp \
    MultiThread.h \
    UI/Settings.h \
    Global/Utils.h \
    Global/IO.h \
    Global/Global.h \
    UI/About.h \
    Global/Version.h \
    Global/log.h \
    Language.h

FORMS    += EterNexus.ui \
    UI/Settings.ui \
    UI/About.ui

LIBS += "$$_PRO_FILE_PWD_\\API\\EterPackAPI.lib"

RESOURCES += "$$_PRO_FILE_PWD_\\Resources\\resources.qrc"
