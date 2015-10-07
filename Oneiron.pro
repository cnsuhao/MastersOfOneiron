LIBS += ../MastersOfOneiron/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

DEFINES += URHO3D_COMPILE_QT

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
    grass.cpp \
    frop.cpp \
    helper.cpp \
    kekelplithf.cpp

HEADERS += \
    mastercontrol.h \
    tile.h \
    slot.h \
    platform.h \
    oneirocam.h \
    inputmaster.h \
    grass.h \
    frop.h \
    helper.h \
    kekelplithf.h
