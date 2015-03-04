QT += core
QT -= widgets gui
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
    calculator.h \
    vector3d.h
    #chartbuilders.h

SOURCES += \
    residue.cpp \
    model.cpp \
    atom.cpp \
    main.cpp \
    chain.cpp \
    pdbparser.cpp \
    calculator.cpp \
    vector3d.cpp
    #chartbuilders.cpp

OTHER_FILES += \
    tutorial.txt

