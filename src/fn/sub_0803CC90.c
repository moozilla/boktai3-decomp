// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct CollisionPoint {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};
struct WirelessObj {
    u8 unknown0[0x52C];
    u8 hitVolume[0x40];
    u8 bodyVolume[0x40];
    u8 proxVolume[1];
};
void WirelessObj_ApplyKnockback();
void sub_0803CB40();
void ScenePool_OnProximityContact();
void SpatialIndex_InitEntry(void *, void *, void *, void *, s32, s32, s32, s32, s32, void (*)(),
                            void *);
void SpatialIndex_InsertEntry(void *);
void WirelessObj_InitCollisionVolumes(struct WirelessObj *p)
{
    struct CollisionPoint pos, size, offset;
    s16 *pp;
    register s32 eight asm("r8");
    pos.x = 0;
    pos.y = -96;
    pos.z = -96;
    pp = (s16 *)&pos;
    size.x = 22;
    size.y = 10;
    size.z = 4;
    offset.x = 0;
    offset.y = -size.y;
    offset.z = 2;
    SpatialIndex_InitEntry((u8 *)p + 0x56C, &pos, &size, &offset, 21, 5, 1, 0, 12,
                           WirelessObj_ApplyKnockback, p);
    SpatialIndex_InsertEntry((u8 *)p + 0x56C);
    ((s16 *)&size)[0] = 16;
    eight = 8;
    asm("" : "+r"(eight));
    ((s16 *)&size)[1] = eight;
    ((s16 *)&size)[2] = 6;
    ((s16 *)&offset)[0] = 0;
    ((s16 *)&offset)[1] = -((s16 *)&size)[1];
    ((s16 *)&offset)[2] = 4;
    SpatialIndex_InitEntry((u8 *)p + 0x52C, &pos, &size, &offset, 12, 5, 1, 1, 12, sub_0803CB40, p);
    pp[0] = 0;
    {
        s16 v = -96;
        pp[1] = v;
        pp[2] = v;
    }
    ((s16 *)&size)[0] = 44;
    ((s16 *)&size)[1] = 10;
    ((s16 *)&size)[2] = eight;
    ((s16 *)&offset)[0] = 0;
    ((s16 *)&offset)[1] = -((s16 *)&size)[1];
    ((s16 *)&offset)[2] = 2;
    SpatialIndex_InitEntry((u8 *)p + 0x860, &pos, &size, &offset, 85, 5, 1, 0, 12,
                           ScenePool_OnProximityContact, p);
    SpatialIndex_InsertEntry((u8 *)p + 0x860);
}
