#-------------------------------------------------
#
# Project created by QtCreator 2014-10-01T13:01:32
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = rsa-keygen
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH+=../../lib/openssl-1.1.0/include ../../lib/jsoncpp/include

LIBS+=-L/Users/Sergej/dfs_lib/lib/openssl-1.1.0 -lcrypto -lssl

SOURCES += main.cpp \
    ../keygen.cpp \
    ../st_rsa_impl.cpp \
    ../commonError.cpp

HEADERS += \
    ../st_rsa_impl.h \
    ../commonError.h
