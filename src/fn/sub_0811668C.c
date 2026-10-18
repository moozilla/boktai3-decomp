// Tentative family and semantic names; layouts follow observed field offsets.
#include "global.h"
struct CollisionPoint {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};
struct RecoilCollisionActor {
    u8 unknown0[0x18];
    u8 owner[0x544 - 0x18];
    u8 hitVolume[0x40];
    u8 bodyVolume[0xac];
    s16 width, height;
    u8 unknown634[0x8f8 - 0x634];
    s8 group;
};
void sub_081162D8();
void ActorRecoil_ApplyDirectionalImpulse();
void SpatialIndex_InitEntry(void *, void *, void *, void *, s32, s32, s32, s32, s32, void (*)(), void *);
void SpatialIndex_InsertEntry(void *);
void ActorRecoil_InitCollisionVolumes(struct RecoilCollisionActor *p)
{
    struct CollisionPoint pos, size, offset;
    void *owner = p->owner;
    s32 group = p->group;
    {
        s32 one = 1;
        group += one;
    }
    pos.x = p->width;
    pos.y = p->height - 96;
    pos.z = p->height - 96;
    size.x = 22;
    size.y = 10;
    size.z = 4;
    offset.x = 0;
    offset.y = -size.y;
    offset.z = 2;
    SpatialIndex_InitEntry(p->bodyVolume, &pos, &size, &offset, 20, group, 0, 0, 12, sub_081162D8, owner);
    ((s16 *)&pos)[0] = p->width;
    ((s16 *)&pos)[1] = p->height - 96;
    ((s16 *)&pos)[2] = p->height - 96;
    ((s16 *)&size)[0] = 16;
    ((s16 *)&size)[1] = 8;
    ((s16 *)&size)[2] = 6;
    ((s16 *)&offset)[0] = 0;
    ((s16 *)&offset)[1] = -((s16 *)&size)[1];
    ((s16 *)&offset)[2] = 4;
    SpatialIndex_InitEntry(p->hitVolume, &pos, &size, &offset, 44, group, 0, 15, 12, ActorRecoil_ApplyDirectionalImpulse, owner);
    SpatialIndex_InsertEntry(p->bodyVolume);
}
