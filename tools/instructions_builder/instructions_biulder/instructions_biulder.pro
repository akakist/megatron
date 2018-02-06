QT += core
QT -= gui

CONFIG += c++11

TARGET = instructions_biulder
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../instructions_builder.cpp
