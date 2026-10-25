#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u8 pad[8];
    u32 flags;
    u8 padc[20];
    struct Vec position;
    u8 pad28[12];
    u8 scaleX, scaleY;
    u16 angle;
};
struct Effect {
    u32 pad0;
    u8 state, entered, pad6, mode;
    u32 item, timer;
    u16 duration;
    u8 index, pad13[5];
    struct Vec position;
    struct Vec *target;
    struct Sprite sprite;
};
extern u8 *gUnknown_02000580[];
extern struct Vec gUnk_03005418;
void FloatingEffect_Init(struct Effect *);
void Sound_PlaySongChecked(u32);
s32 Inventory_IsKeyItemId(u32);
static inline int FloatingEffect_Scale3(int v)
{
    int t = v * 3;
    return t >= 0 ? t >> 5 : -((-t) >> 5);
}
static inline int FloatingEffect_Scale4(int v)
{
    return v >= 0 ? v >> 4 : -((-v) >> 4);
}
static inline void FloatingEffect_ProjectVec(struct Vec *out, struct Vec *v)
{
    int depth, height;
    int vx = v->x, vz = v->z;
    out->x = FloatingEffect_Scale3(vx - vz);
    depth = FloatingEffect_Scale3(vx + vz);
    height = FloatingEffect_Scale3(v->y);
    out->y = depth - height;
    out->z = depth + height;
    out->x = out->x - gUnk_03005418.x + 120;
    out->y = out->y - gUnk_03005418.y + 80;
    out->z = out->z - gUnk_03005418.z;
}
#ifdef NONMATCHING
// fuzzy: 97.3%
// scope shape 6 (differences once low registers are set aside)
// scope shape 54 (differences once low registers are set aside)
void FloatingItemEffect_UpdateFlight(u8 *m, struct Effect *p, int index)
{
    struct Sprite *s = &p->sprite;
    struct Vec to, from;
    u32 t, inv;
    u8 *scale;
    int xl;
    if (p->entered) {
        p->entered = 0;
        p->sprite.flags &= ~1;
        s->flags |= 2;
        s->angle = 0;
        p->sprite.scaleX = 64;
        p->sprite.scaleY = 64;
    }
    t = p->timer;
    inv = 16 - t;
    if (p->mode == 0)
        FloatingEffect_ProjectVec(&to, (struct Vec *)(gUnknown_02000580[p->index] + 0x30));
    else
        FloatingEffect_ProjectVec(&to, p->target);
    FloatingEffect_ProjectVec(&from, &p->position);
    to.y -= 4;
    s->position.x = FloatingEffect_Scale4(from.x * inv + to.x * t);
    s->position.y = FloatingEffect_Scale4(from.y * inv + to.y * t);
    scale = &s->scaleX;
    if (*scale > 32)
        *scale -= 2;
    s->scaleY = *scale;
    s->angle += 8;
    s->position.x -= 8;
    s->position.x += (64 - *scale) >> 3;
    s->position.y -= 8;
    if (p->timer > 15) {
        if (p->pad6 == 0) {
            if (p->item - 40 <= 6)
                Sound_PlaySongChecked(0x183);
            else if (Inventory_IsKeyItemId(p->item) == 1)
                Sound_PlaySongChecked(0x123);
            else
                Sound_PlaySongChecked(0x111);
        } else if (p->pad6 != 3) {
            Sound_PlaySongChecked(0x123);
        }
        FloatingEffect_Init(p);
        *(u32 *)(m + 24) &= ~(1 << index);
    } else
        p->timer++;
}
#else
INCLUDE_ASM("asm/nonmatching", FloatingItemEffect_UpdateFlight);
#endif
