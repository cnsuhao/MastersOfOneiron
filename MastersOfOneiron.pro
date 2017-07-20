TARGET = moo

LIBS += ../MastersOfOneiron/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += \
    ../MastersOfOneiron/Urho3D/include \
    ../MastersOfOneiron/Urho3D/include/Urho3D/ThirdParty \

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    mastercontrol.cpp \
    tile.cpp \
    slot.cpp \
    platform.cpp \
    oneirocam.cpp \
    inputmaster.cpp \
    controllable.cpp \
    spawnmaster.cpp \
    sceneobject.cpp \
    resourcemaster.cpp \
    grass.cpp \
    frop.cpp \
    luckey.cpp \
    kekelplithf.cpp

HEADERS += \
    mastercontrol.h \
    tile.h \
    slot.h \
    platform.h \
    oneirocam.h \
    inputmaster.h \
    controllable.h \
    spawnmaster.h \
    sceneobject.h \
    resourcemaster.h \
    grass.h \
    frop.h \
    luckey.h \
    kekelplithf.h
