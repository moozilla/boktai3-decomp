#include "bosses/vanargand/sprite_trail.h"

struct PackedCoordinate {
    u32 lo : 16, hi : 16;
};
struct PackedVector {
    struct PackedCoordinate xy, z;
};
void AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_InitUnlinked(void *, void *, s32);
void Position_GetHeightAdjustedCoordinates(void *, s32, void *);
void SpatialIndex_InitEntry(void *, struct PackedVector *, struct PackedVector *,
                            struct PackedVector *, u32, u32, u32, u32, u32, void (*)(void *),
                            void *);
void SpatialIndex_InsertEntry(void *);
void MotionEffectPool_OnRenderComplete(void *);
void SingletonSlot7_OnEffectMotionComplete(void *);

s32 SpriteTrail_InitializeEntry(void *pool, struct SpriteTrail *p, s32 index)
{
    struct DisplaySequenceSprite *sprite;
    s32 i;
    struct PackedVector position, size, offset;
    void *collider;

    p->state = 0;
    p->entered = 1;
    p->frame = 0;
    AnimData_Load(&p->graphics, 0x89CB);
    AnimData_SelectPaletteBank(&p->graphics, 0x1DA);
    sprite = p->head;
    for (i = 0; i <= 3; i++, sprite++) {
        Sprite_InitUnlinked(sprite, &p->graphics, 0x11);
        sprite->frame = 0x26 - (4 - i) * 2;
        sprite->priority = 1;
        ZeroFill8(&sprite->position);
    }
    sprite = p->tail;
    for (i = 0; i <= 2; i++, sprite++) {
        Sprite_InitUnlinked(sprite, &p->graphics, 0x11);
        sprite->frame = 0x25 - (3 - i) * 2;
        sprite->priority = 1;
        ZeroFill8(&sprite->position);
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
