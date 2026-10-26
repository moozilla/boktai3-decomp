#define gSineTable           gUnk_086149C4
#define gPlayerPositionOwner gGameState
#include "global.h"
struct LinkVector {
    s16 x, y, z;
};
struct LinkEffect {
    u8 pad0[0x20];
    struct LinkVector *target;
    struct LinkVector start;
    struct LinkVector end;
};
struct LinkPlayer {
    u8 pad0[0x30];
    struct LinkVector position;
};
extern struct LinkPlayer *gPlayerPositionOwner;
extern const s16 gSineTable[];
s32 Math_Atan2(s32, s32);
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void polar(struct LinkVector *v, s32 angle, s32 radius)
{
    v->x = scale(gSineTable[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gSineTable[angle & 255] * radius);
}
static inline void add(struct LinkVector *dst, struct LinkVector *a, struct LinkVector *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}
void LinkEffect_UpdateEndpoints(struct LinkEffect *p)
{
    struct LinkVector offset;
    s32 angle = Math_Atan2(p->target->x - gPlayerPositionOwner->position.x,
                           p->target->z - gPlayerPositionOwner->position.z);
    s32 opposite = (angle + 128) & 255;
    polar(&offset, angle, 256);
    offset.y = 128;
    add(&p->start, &offset, p->target);
    polar(&offset, opposite, 256);
    offset.y = 128;
    p->end.x = gPlayerPositionOwner->position.x + offset.x;
    p->end.y = gPlayerPositionOwner->position.y + offset.y;
    p->end.z = gPlayerPositionOwner->position.z + offset.z;
}
