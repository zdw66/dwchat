QT       += core gui network multimedia multimediawidgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    chatclient.cpp

HEADERS += \
    chatclient.h

FORMS += \
    chatclient.ui

LIBS += /usr/local/lib64/libopencv*

INCLUDEPATH += /usr/local/include \
                /usr/local/include/opencv4 \
                /usr/local/include/opencv4/opencv2

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-DWChat-Desktop_Qt_5_12_9_GCC_64bit-Debug/Algorithm/release/ -lAlgorithm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-DWChat-Desktop_Qt_5_12_9_GCC_64bit-Debug/Algorithm/debug/ -lAlgorithm
else:unix: LIBS += -L$$PWD/../../build-DWChat-Desktop_Qt_5_12_9_GCC_64bit-Debug/Algorithm/ -lAlgorithm

INCLUDEPATH += $$PWD/../Algorithm
#INCLUDEPATH += $$PWD/../Algorithm/chatmessage
DEPENDPATH += $$PWD/../Algorithm
