QT += core
QT -= widgets
TEMPLATE = app
TARGET = Fragmenter

CONFIG   += console
CONFIG   -= app_bundle


HEADERS += \
    residue.h \
    model.h \
    atom.h \
    chain.h \
    pdbparser.h \
    calculator.h
    #chartbuilders.h

SOURCES += \
    residue.cpp \
    model.cpp \
    atom.cpp \
    main.cpp \
    chain.cpp \
    pdbparser.cpp \
    calculator.cpp
    #chartbuilders.cpp

OTHER_FILES += \
    tutorial.txt

