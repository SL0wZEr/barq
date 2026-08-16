QT += core gui qml quick quickcontrols2 dbus network

CONFIG += c++17 release
TARGET = barq
TEMPLATE = app

HEADERS += \
    src/backend.h \
    src/singleinstance.h \
    src/systemtheme.h

SOURCES += \
    src/main.cpp \
    src/backend.cpp \
    src/singleinstance.cpp \
    src/systemtheme.cpp

RESOURCES += src/resources.qrc
