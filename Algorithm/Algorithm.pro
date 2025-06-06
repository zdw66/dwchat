QT += gui network sql
TEMPLATE = lib
DEFINES += ALGORITHM_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    algorithm.cpp \
    archive.cpp \
    auth.cpp \
    authcode.cpp \
    chatbot.cpp \
    chatmessage/fileview.cpp \
    chatmessage/myfileinfo.cpp \
    chatmessage/qnchatmessage.cpp \
    emijo.cpp \
    encipher.cpp \
    wxlogin.cpp

HEADERS += \
    Algorithm_global.h \
    algorithm.h \
    archive.h \
    auth.h \
    authcode.h \
    chatbot.h \
    chatmessage/fileview.h \
    chatmessage/myfileinfo.h \
    chatmessage/qnchatmessage.h \
    emijo.h \
    encipher.h \
    wxlogin.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

# 添加 cmark 的库路径（如果必要）
INCLUDEPATH += /usr/local/include  # 或者 cmark 的安装路径
LIBS += -L/usr/local/lib64 -lcmark

FORMS += \
    emijo.ui \
    fileview.ui

DISTFILES += \
    app.config
