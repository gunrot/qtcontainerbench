#-------------------------------------------------
#
# Project created by QtCreator 2015-04-25T15:47:26
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

CONFIG +=c++11
#QMAKE_CXXFLAGS_RELEASE -= -O2
#QMAKE_CXXFLAGS_RELEASE += -O3 -march=native -mtune=native -flto

#QMAKE_LFLAGS_RELEASE -= -O1

TARGET = tst_map_hast_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
INCLUDEPATH += ../boost

SOURCES += tst_map_hast_test.cpp \
    qfasthash.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

#DEFINES += TEST_KEY_STRING
#DEFINES += TEST_KEY_DOUBLE
DEFINES += TEST_KEY_INT32

DEFINES += USE_BOOST
HEADERS += \
    qfasthash_p.h
