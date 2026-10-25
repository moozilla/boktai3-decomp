#include "global.h"

struct TrailVector {
    s32 x, y, z, pad;
};
struct TrailSprite {
    u32 flags;
    u8 unk_04[3];
    u8 enabled;
    u8 unk_08[8];
    u16 order;
    u8 unk_12[10];
    s16 x, y, z, pad;
    u8 unk_24[8];
};
struct SpriteTrail {
    u8 entered, state;
    u8 unk_02[2];
    struct TrailVector position, history[4], velocity, target;
    u32 style, collisionArg0, collisionArg1, age, frame;
    u8 collider[0x40], renderCollider[0x40];
    struct TrailSprite head[4], tail[3];
    u8 animation[0x1c];
};
struct PackedCoordinate {
    u32 lo : 16, hi : 16;
};
struct PackedVector {
    struct PackedCoordinate xy, z;
};
void AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_InitUnlinked(void *, void *, s32);
void CpuSet(const void *, void *, u32);
void Position_GetHeightAdjustedCoordinates(void *, s32, void *);
void SpatialIndex_InitEntry(void *, struct PackedVector *, struct PackedVector *,
                            struct PackedVector *, u32, u32, u32, u32, u32, void (*)(void *),
                            void *);
void SpatialIndex_InsertEntry(void *);
void MotionEffectPool_OnRenderComplete(void *);
void SingletonSlot7_OnEffectMotionComplete(void *);

static inline void SpriteTrail_ClearSpritePosition(void *position)
{
    vu32 zero = 0;
    CpuSet((const void *)&zero, position, 0x05000002);
}

s32 SpriteTrail_InitializeEntry(void *pool, struct SpriteTrail *p, s32 index)
{
    struct TrailSprite *sprite;
    s32 i;
    struct PackedVector position, size, offset;
    void *collider;

    p->state = 0;
    p->entered = 1;
    p->frame = 0;
    AnimData_Load(p->animation, 0x89CB);
    AnimData_SelectPaletteBank(p->animation, 0x1DA);
    sprite = p->head;
    for (i = 0; i <= 3; i++, sprite++) {
        Sprite_InitUnlinked(sprite, p->animation, 0x11);
        sprite->order = 0x26 - (4 - i) * 2;
        sprite->enabled = 1;
        SpriteTrail_ClearSpritePosition(&sprite->x);
    }
    sprite = p->tail;
    for (i = 0; i <= 2; i++, sprite++) {
        Sprite_InitUnlinked(sprite, p->animation, 0x11);
        sprite->order = 0x25 - (3 - i) * 2;
        sprite->enabled = 1;
        SpriteTrail_ClearSpritePosition(&sprite->x);
    }
    collider = p->renderCollider;
    Position_GetHeightAdjustedCoordinates(&p->position, 0, &position);
    size.xy.lo = 4;
    size.xy.hi = 4;
    size.z.lo = 4;
    *(u32 *)&offset.xy = 0;
    *(u32 *)&offset.z &= 0xFFFF0000;
    SpatialIndex_InitEntry(collider, &position, &size, &offset, 20, 5, 1, 0, 0,
                           MotionEffectPool_OnRenderComplete, p);
    SpatialIndex_InsertEntry(collider);
    collider = p->collider;
    Position_GetHeightAdjustedCoordinates(&p->position, 0, &position);
    size.xy.lo = 4;
    size.xy.hi = 4;
    size.z.lo = 4;
    offset.xy.lo = 0;
    offset.xy.hi = 0;
    offset.z.lo = 0;
    SpatialIndex_InitEntry(collider, &position, &size, &offset, 12, 5, 1, p->collisionArg0,
                           p->collisionArg1, SingletonSlot7_OnEffectMotionComplete, p);
}
