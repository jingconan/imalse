/****************************************************************************
** Meta object code from reading C++ file 'animatorscene.h'
**
** Created: Mon Jul 30 23:00:33 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "animator/animatorscene.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'animatorscene.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_netanim__AnimatorScene[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x09,
      48,   23,   23,   23, 0x09,
      73,   23,   23,   23, 0x09,
     109,   23,   23,   23, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_netanim__AnimatorScene[] = {
    "netanim::AnimatorScene\0\0applyPacketFilterSlot()\0"
    "applyNodePosFilterSlot()\0"
    "packetFilterProtocolSelectAllSlot()\0"
    "packetFilterProtocolDeSelectAllSlot()\0"
};

const QMetaObject netanim::AnimatorScene::staticMetaObject = {
    { &QGraphicsScene::staticMetaObject, qt_meta_stringdata_netanim__AnimatorScene,
      qt_meta_data_netanim__AnimatorScene, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &netanim::AnimatorScene::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *netanim::AnimatorScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *netanim::AnimatorScene::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_netanim__AnimatorScene))
        return static_cast<void*>(const_cast< AnimatorScene*>(this));
    return QGraphicsScene::qt_metacast(_clname);
}

int netanim::AnimatorScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: applyPacketFilterSlot(); break;
        case 1: applyNodePosFilterSlot(); break;
        case 2: packetFilterProtocolSelectAllSlot(); break;
        case 3: packetFilterProtocolDeSelectAllSlot(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
