isEmpty(PREFIX) {
    PREFIX = /usr/local
}

TEMPLATE = app
TARGET = indicator-stock-trend
VERSION = 0.0.11

INCLUDEPATH += .

QT += widgets
QT += network

CONFIG += link_pkgconfig
PKGCONFIG += dqtxAppIndicator
PKGCONFIG += dqtx

QMAKE_CXXFLAGS += -std=c++11

# Input
HEADERS += Application.hpp
SOURCES += Application.cpp main.cpp

# install
target.path = $$PREFIX/bin
INSTALLS += target
