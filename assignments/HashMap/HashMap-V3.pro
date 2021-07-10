QT -= gui

CONFIG += c++17 console
QMAKE_CXXFLAGS += -O0 -Wfatal-errors -Wall -Wextra
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        hashmap.cpp \
        main.cpp \
        student_main.cpp \
        test_settings.cpp \
        tests.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    hashmap.h \
    hashmap_iterator.h

DISTFILES += \
    short_answer.txt
