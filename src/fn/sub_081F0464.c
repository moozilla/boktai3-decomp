#define ActorCollider_Initialize SpatialIndex_InitEntry
#define ActorCollider_Activate   SpatialIndex_InsertEntry
#include "global.h"
struct Slot7PackedVector {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
struct Slot7Actor {
    u8 pad0[0xb3];
    u8 mode;
    u8 padb4[0xc8];
    u32 secondaryColliderFlags;
    u8 pad180[0xe0];
    struct Slot7PackedVector position;
    s32 x;
    u8 pad26c[0x358];
    s16 parameter5c4, parameter5c6;
};
void ScreenPhase_DampenMotion(void);
void SpatialIndex_InitEntry(void *, struct Slot7PackedVector *, struct Slot7PackedVector *,
                            struct Slot7PackedVector *, s32, s32, s32, s32, s32, void (*)(void),
                            void *);
void SpatialIndex_InsertEntry(void *);
void SingletonSlot7_InitSecondaryCollider(struct Slot7Actor *actor)
{
    void *collider = &actor->secondaryColliderFlags;
    struct Slot7PackedVector position, size, offset;
    if (actor->mode != 2) {
        position.x = actor->x >> 12;
        position.y = actor->x >> 12;
        position.z = actor->x >> 12;
    } else {
        position = actor->position;
    }
    size.x = 80;
    size.y = 120;
    size.z = 55;
    offset.x = -100;
    offset.y = 60;
    offset.z = 0;
    SpatialIndex_InitEntry(collider, &position, &size, &offset, 20, 5, 1, actor->parameter5c4,
                           actor->parameter5c6, ScreenPhase_DampenMotion, actor);
    SpatialIndex_InsertEntry(collider);
}
