#define gSineTable gUnk_086149C4
#include "global.h"
struct ColliderVector {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct RatatoskPosition {
    s16 x, y, z, pad;
};
struct RatatoskFacing {
    u8 pad[9];
    u8 direction;
};
struct Ratatosk {
    u8 pad0[0x48];
    struct RatatoskFacing facing;
    u8 pad54[0x1dc];
    u8 frontCollider[0x13c];
    struct RatatoskPosition position;
    u8 pad374[0xb2e];
    s16 attack, damage, attackKind;
};
extern const s16 gUnk_086149C4[];
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Ratatosk_OnFrontCollision(void);
static inline s32 scale(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct RatatoskPosition *v, s32 angle, s32 radius)
{
    v->x = scale(gSineTable[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gSineTable[angle & 255] * radius);
}
void Ratatosk_InitFrontCollider(struct Ratatosk *p)
{
    struct RatatoskFacing *facing = &p->facing;
    void *collider = p->frontCollider;
    struct ColliderVector size, offset;
    struct RatatoskPosition pos;
    size.x = 96;
    size.y = 128;
    size.z = 96;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    polar(&pos, (u8)((facing->direction << 6) - 64), 512);
    {
        struct RatatoskPosition *v = &pos;
        struct RatatoskPosition *base = &p->position;
        v->x = v->x + base->x;
        v->y = v->y + base->y;
        v->z = v->z + base->z;
    }
    Collider_SetPosition(collider, &pos, 0);
    Collider_SetCallback(collider, Ratatosk_OnFrontCollision, p);
    Collider_SetAttackProperties(collider, *(s16 *)((u8 *)p + 0xea2), *(s16 *)((u8 *)p + 0xea4), 0,
                                 0x40000, *(s16 *)((u8 *)p + 0xea6));
}
