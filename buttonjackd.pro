TARGET = buttonjackd

QT += dbus
QT -= gui

DEFINES += "APPVERSION=\\\"$${SPECVERSION}\\\""

target.path = /usr/bin/

systemduser.files = config/buttonjackd.service
systemduser.path = /etc/systemd/user/

INSTALLS += target systemduser

message($${DEFINES})

SOURCES += \
    src/buttonjackd.cpp \
    src/uinputevpoll.cpp \
    src/buttonmanager.cpp \
    src/volumecontrol.cpp \
    src/mpriscontrol.cpp \
    src/callhandler.cpp

OTHER_FILES += \
    rpm/$${TARGET}.spec \
    config/buttonjackd.service

HEADERS += \
    src/uinputevpoll.h \
    src/buttonmanager.h \
    src/volumecontrol.h \
    src/mpriscontrol.h \
    src/callhandler.h
