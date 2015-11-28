QT += core
QT -= gui

TARGET = MonotonePolygonTriangulation
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

SOURCES += main.cpp \
    polypartition.cpp

HEADERS += \
    polypartition.h

