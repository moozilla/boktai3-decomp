#include "engine/shared/global.h"
#include "engine/shared/contact_position.h"

struct WirelessObj {
    u8 unknown0[0x52C];
    u8 hitVolume[0x40];
    u8 bodyVolume[0x40];
    u8 unknown5AC[0x2B4];
    u8 proxVolume[0x40];
};
void WirelessObj_ApplyKnockback();
void sub_0803CB40();
void ScenePool_OnProximityContact();
void SpatialIndex_InitEntry(void *, void *, void *, void *, s32, s32, s32, s32, s32, void (*)(),
                            void *);
void SpatialIndex_InsertEntry(void *);
void WirelessObj_InitCollisionVolumes(struct WirelessObj *p)
{
    struct ContactPosition pos, size, offset;

    pos.x = 0;
    pos.y = -96;
    pos.z = -96;
    size.x = 22;
    size.y = 10;
    size.z = 4;
    offset.x = 0;
    offset.y = -size.y;
    offset.z = 2;
    SpatialIndex_InitEntry(p->bodyVolume, &pos, &size, &offset, 21, 5, 1, 0, 12,
                           WirelessObj_ApplyKnockback, p);
    SpatialIndex_InsertEntry(p->bodyVolume);
    size.x = 16;
    size.y = 8;
    size.z = 6;
    offset.x = 0;
    offset.y = -size.y;
    offset.z = 4;
    SpatialIndex_InitEntry(p->hitVolume, &pos, &size, &offset, 12, 5, 1, 1, 12, sub_0803CB40, p);
    pos.x = 0;
    pos.y = -96;
    pos.z = -96;
    size.x = 44;
    size.y = 10;
    size.z = 8;
    offset.x = 0;
    offset.y = -size.y;
    offset.z = 2;
    SpatialIndex_InitEntry(p->proxVolume, &pos, &size, &offset, 85, 5, 1, 0, 12,
                           ScenePool_OnProximityContact, p);
    SpatialIndex_InsertEntry(p->proxVolume);
}
