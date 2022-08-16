QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 lrelease

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    blocktablemodel.cpp \
    driver2block.cpp \
    driverblock.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    abstractblock.h \
    blocktablemodel.h \
    driver2block.h \
    driverblock.h \
    dtypes.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += translations/blocks/blocks_ru.ts

#Windows only
RC_ICONS = image/logo.ico
QMAKE_TARGET_DESCRIPTION = Blocks

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
