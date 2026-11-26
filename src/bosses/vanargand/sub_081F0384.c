#define ActorCollider_Initialize SpatialIndex_InitEntry
#define ActorCollider_Activate   SpatialIndex_InsertEntry
#include "global.h"
struct VanargandHeadPackedVector {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
struct VanargandHeadActorView {
    u8 pad0[0x1bc];
    u32 colliderFlags;
    u8 pad1c0[0xa8];
    s32 x, y, z;
    u8 pad274[0x350];
    s16 parameter5c4, parameter5c6;
};
void ActorReaction_MarkSequenceHit(void);
void SpatialIndex_InitEntry(void *, struct VanargandHeadPackedVector *, struct VanargandHeadPackedVector *,
                            struct VanargandHeadPackedVector *, s32, s32, s32, s32, s32, void (*)(void),
                            void *);
void SpatialIndex_InsertEntry(void *);
void VanargandHead_InitCollider(struct VanargandHeadActorView *actor)
{
    void *collider = &actor->colliderFlags;
    s32 category = 5;
    struct VanargandHeadPackedVector position, size, offset;
    position.x = actor->x >> 12;
    position.y = actor->y >> 12;
    position.z = actor->z >> 12;
    size.x = 80;
    size.y = 130;
    size.z = 55;
    offset.x = -80;
    offset.y = 60;
    offset.z = 0;
    SpatialIndex_InitEntry(collider, &position, &size, &offset, 12, category, 1,
                           actor->parameter5c4, actor->parameter5c6, ActorReaction_MarkSequenceHit,
                           actor);
    SpatialIndex_InsertEntry(collider);
    actor->colliderFlags |= 1;
}
