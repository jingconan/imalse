HEADERS += \
    animator/mobilitymanager.h \
    animator/animxmlparser.h \
    animator/animpacket.h \
    animator/animnode.h \
    animator/animatorview.h \
    animator/animatorscene.h \
    debug/xdebug.h \
    animator/animatormode.h \
    main/netanim.h \
    animator/packetstatisticsdialog.h \
    animator/nodepositionstatisticsdialog.h \
    animator/nodetrajectorydialog.h



SOURCES += \
    animator/mobilitymanager.cpp \
    animator/animxmlparser.cpp \
    animator/animpacket.cpp \
    animator/animnode.cpp \
    animator/animatorview.cpp \
    animator/animatorscene.cpp \
    debug/xdebug.cpp \
    animator/main.cpp \
    animator/animatormode.cpp \
    main/netanim.cpp \
    animator/packetstatisticsdialog.cpp \
    animator/nodepositionstatisticsdialog.cpp \
    animator/nodetrajectorydialog.cpp


RESOURCES += \
    animator/animator.qrc

QT += svg

macx {
 CONFIG -= app_bundle
 QMAKESPEC = macx-g++
}

# DEFINES +="Q_MOC_OUTPUT_REVISION=62"

### Uncomment the below line to enable ns-3 related features
# include(ns3.pro)












































