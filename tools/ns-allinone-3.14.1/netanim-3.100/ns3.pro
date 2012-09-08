######### NS-3 related
DEFINES += WITH_NS3


HEADERS += \
    designer/designermode.h \
    designer/designerview.h \
    designer/designerscene.h \
    designer/designernoderect.h \
    designer/abstractdesigneritem.h \
    designer/designerp2plinkline.h \
    designer/designerconstants.h \
    designer/nodepropertiestab.h \
    designer/p2plinkmanager.h \
    designer/nodenetworkwidget.h \
    designer/appmanager.h \
    designer/designermodule.h \
    designer/applicationswidget.h \
    designer/attributerandomvariablewidget.h \
    designer/attributeipv4addrportwidget.h \
    designer/attributeprotocolwidget.h

SOURCES += \
    designer/designermode.cpp \
    designer/designerview.cpp \
    designer/designerscene.cpp \
    designer/designernoderect.cpp \
    designer/abstractdesigneritem.cpp \
    designer/designerp2plinkline.cpp \
    designer/nodepropertiestab.cpp \
    designer/p2plinkmanager.cpp \
    designer/nodenetworkwidget.cpp \
    designer/appmanager.cpp \
    designer/applicationswidget.cpp \
    designer/attributerandomvariablewidget.cpp \
    designer/attributeipv4addrportwidget.cpp \
    designer/attributeprotocolwidget.cpp

CONFIG+=link_pkgconfig
PKGCONFIG+=libns3-aodv
PKGCONFIG+=libns3-ns3tcp
PKGCONFIG+=libns3-applications
PKGCONFIG+=libns3-wifi
PKGCONFIG+=libns3-bridge
PKGCONFIG+=libns3-olsr
PKGCONFIG+=libns3-config-store
PKGCONFIG+=libns3-point-to-point-layout
PKGCONFIG+=libns3-core
PKGCONFIG+=libns3-point-to-point
PKGCONFIG+=libns3-csma-layout
PKGCONFIG+=libns3-propagation
PKGCONFIG+=libns3-csma
PKGCONFIG+=libns3-spectrum
PKGCONFIG+=libns3-dsdv
PKGCONFIG+=libns3-stats
PKGCONFIG+=libns3-energy
PKGCONFIG+=libns3-template
PKGCONFIG+=libns3-flow-monitor
PKGCONFIG+=libns3-test
PKGCONFIG+=libns3-internet
PKGCONFIG+=libns3-tools
PKGCONFIG+=libns3-lte
PKGCONFIG+=libns3-topology-read
PKGCONFIG+=libns3-mesh
PKGCONFIG+=libns3-uan
PKGCONFIG+=libns3-mobility
PKGCONFIG+=libns3-virtual-net-device
PKGCONFIG+=libns3-mpi
PKGCONFIG+=libns3-visualizer
PKGCONFIG+=libns3-netanim
PKGCONFIG+=libns3-wifi
PKGCONFIG+=libns3-network
PKGCONFIG+=libns3-wimax
PKGCONFIG+=libns3-nix-vector-routing


DEFINES += _DEBUG
DEFINES += NS3_ASSERT_ENABLE
DEFINES += NS3_LOG_ENABLE

