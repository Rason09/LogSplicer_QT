QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LogSplicer.cpp \
    Logger.cpp \
    MD5Checker.cpp \
    Product.cpp \
    Task.cpp \
    Worker.cpp \
    main.cpp \
    mainwindow.cpp \
    md5_dialog.cpp

HEADERS += \
    DataDefines.h \
    LogSplicer.h \
    Logger.h \
    MD5Checker.h \
    Product.h \
    Task.h \
    ThreadPool.hpp \
    Worker.h \
    mainwindow.h \
    md5_dialog.h

FORMS += \
    mainwindow.ui \
    md5_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    LogSplicer.qrc

RC_FILE = star.rc
