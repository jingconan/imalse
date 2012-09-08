/****************************************************************************
** Meta object code from reading C++ file 'animatormode.h'
**
** Created: Mon Jul 30 23:00:34 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "animator/animatormode.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'animatormode.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_netanim__AnimatorMode[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      29,   23,   22,   22, 0x08,
      48,   23,   22,   22, 0x08,
      70,   22,   22,   22, 0x08,
      81,   22,   22,   22, 0x08,
     101,   22,   22,   22, 0x08,
     112,   22,   22,   22, 0x08,
     123,   22,   22,   22, 0x08,
     140,   22,   22,   22, 0x08,
     167,   22,   22,   22, 0x08,
     191,   22,   22,   22, 0x08,
     204,   22,   22,   22, 0x08,
     218,   22,   22,   22, 0x08,
     244,   22,   22,   22, 0x08,
     262,   22,   22,   22, 0x08,
     286,   23,   22,   22, 0x08,
     311,  304,   22,   22, 0x08,
     338,  304,   22,   22, 0x08,
     363,   22,   22,   22, 0x08,
     380,   22,   22,   22, 0x08,
     396,   22,   22,   22, 0x08,
     407,   22,   22,   22, 0x08,
     419,   22,   22,   22, 0x08,
     433,   22,   22,   22, 0x08,
     448,   23,   22,   22, 0x08,
     479,   23,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_netanim__AnimatorMode[] = {
    "netanim::AnimatorMode\0\0value\0"
    "gridLinesSlot(int)\0nodeSizeSlot(QString)\0"
    "gridSlot()\0traceFileOpenSlot()\0"
    "testSlot()\0playSlot()\0showNodeIdSlot()\0"
    "updateRateChangedSlot(int)\0"
    "updateRateTimeoutSlot()\0zoomInSlot()\0"
    "zoomOutSlot()\0showWirelessCirclesSlot()\0"
    "packetStatsSlot()\0nodePositionStatsSlot()\0"
    "timelineSlot(int)\0nodeId\0"
    "pktFilterFromSlot(QString)\0"
    "pktFilterToSlot(QString)\0showPacketSlot()\0"
    "precisionSlot()\0saveSlot()\0resetSlot()\0"
    "fastFwdSlot()\0showMetaSlot()\0"
    "packetPersistenceSlot(QString)\0"
    "metaFontSizeSlot(int)\0"
};

const QMetaObject netanim::AnimatorMode::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_netanim__AnimatorMode,
      qt_meta_data_netanim__AnimatorMode, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &netanim::AnimatorMode::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *netanim::AnimatorMode::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *netanim::AnimatorMode::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_netanim__AnimatorMode))
        return static_cast<void*>(const_cast< AnimatorMode*>(this));
    return QWidget::qt_metacast(_clname);
}

int netanim::AnimatorMode::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: gridLinesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: nodeSizeSlot((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: gridSlot(); break;
        case 3: traceFileOpenSlot(); break;
        case 4: testSlot(); break;
        case 5: playSlot(); break;
        case 6: showNodeIdSlot(); break;
        case 7: updateRateChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: updateRateTimeoutSlot(); break;
        case 9: zoomInSlot(); break;
        case 10: zoomOutSlot(); break;
        case 11: showWirelessCirclesSlot(); break;
        case 12: packetStatsSlot(); break;
        case 13: nodePositionStatsSlot(); break;
        case 14: timelineSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: pktFilterFromSlot((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 16: pktFilterToSlot((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 17: showPacketSlot(); break;
        case 18: precisionSlot(); break;
        case 19: saveSlot(); break;
        case 20: resetSlot(); break;
        case 21: fastFwdSlot(); break;
        case 22: showMetaSlot(); break;
        case 23: packetPersistenceSlot((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 24: metaFontSizeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 25;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
