#include "global.h"
struct TrailVector {
    s32 x, y, z, w;
};
struct TrailPoint {
    s32 x, y, z, w;
};
struct SpriteTrail {
    u8 entered, state;
    u8 pad2[2];
    struct TrailPoint position;
    struct TrailPoint points[4];
    struct TrailVector velocity;
    u8 pad64[0x74 - 0x64];
    s32 scroll;
    u8 pad78[0x84 - 0x78];
    s32 age;
    u8 collider[0x40];
    u32 spriteFlags;
};
void Vector3_CopyComponents(struct TrailVector *, struct TrailVector *);
void Vector3_AddComponents(void *, void *, struct TrailVector *);
void Collider_SetFixedPositionWithYOffset(void *, void *, s32);
void SpatialIndex_InsertEntry(void *);
void SpriteTrail_UpdatePositions(struct SpriteTrail *);
static inline void SpriteTrail_SetState(struct SpriteTrail *p, u8 state)
{
    p->state = state;
    p->entered = 1;
    p->age = 0;
}
// confidence=low: moves a falling trail (slowly for its first 32 frames) and its points with
// the scroll; it bursts after 120 frames or lands once y turns positive.
void sub_081F25A4(void *unused, struct SpriteTrail *p)
{
    struct TrailVector step;
    s32 i;
    if (p->entered) {
        p->entered = 0;
        {
            u32 *flags = &p->spriteFlags;
            s32 mask = ~1;
            *flags &= mask;
        }
    }
    Vector3_CopyComponents(&step, &p->velocity);
    if (p->age < 0x20) {
        step.x >>= 3;
        step.y >>= 3;
        step.z >>= 3;
    }
    step.x += p->scroll;
    Vector3_AddComponents(&p->position, &p->position, &step);
    for (i = 0; i < 4; i++)
        p->points[i].x += p->scroll;
    Collider_SetFixedPositionWithYOffset(&p->spriteFlags, &p->position, 0);
    Collider_SetFixedPositionWithYOffset(p->collider, &p->position, 0);
    SpatialIndex_InsertEntry(p->collider);
    SpriteTrail_UpdatePositions(p);
    if (p->age > 0x77)
        SpriteTrail_SetState(p, 2);
    else if (p->position.y > 0)
        SpriteTrail_SetState(p, 1);
    else
        p->age++;
}
