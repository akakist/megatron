#-------------------------------------------------
#
# Project created by QtCreator 2014-10-01T13:01:32
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = blockchain_utils
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH+=/Users/Sergej/dfs_lib/lib/openssl-1.1.0/include ../../lib/jsoncpp/include ../../../../../megatron/lib/jsoncpp-0.5.0/include . ..

LIBS+=-L/Users/Sergej/dfs_lib/lib/openssl-1.1.0 -lcrypto -lssl

SOURCES += main.cpp \
    ../keygen.cpp \
    ../st_rsa_impl.cpp \
    ../commonError.cpp \
    ../../../../../megatron/lib/jsoncpp-0.5.0/src/lib_json/json_reader.cpp \
    ../../../../../megatron/lib/jsoncpp-0.5.0/src/lib_json/json_value.cpp \
    ../../../../../megatron/lib/jsoncpp-0.5.0/src/lib_json/json_writer.cpp \

HEADERS += \
    ../st_rsa_impl.h \
    ../commonError.h
