#include "global.h"
/* Tentative role: place and launch a directional effect. */
typedef struct Point {
    u16 x, z, y, pad6;
} Point;
typedef struct Sprite {
    u8 pad0[0x1c];
    Point pos;
} Sprite;
typedef struct State {
    u8 pad0[0x3a4];
    u8 facing;
    u8 pad3a5[0x457];
    Sprite sprite;
    u8 pad820[0x20];
    u32 pad840;
    u8 animation[0x10];
    void *resource;
    s16 dx;
    u8 pad85a[2];
    s16 dy;
} State;
extern const s16 gUnk_086149C4[];
void DirectionalEffect_DecodeAnimationMode(u32, u32 *, u32 *);
void Anim_SetSequence(void *, void *, s32, u8, u8);
void Player_ShowDirectionalEffect(State *);
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

// confidence=low: Player_StartDirectionalEffect with a size class: 0 = lift 160, radius 450;
// 1 = lift 140, radius 338; otherwise lift 120, radius 224.
void sub_08238CE0(State *s, u32 mode, Point *pos, u32 size)
{
    Sprite *sprite = &s->sprite;
    s32 angle;
    u32 a, b;
    s32 lift, radius;
    if (size == 0) {
        lift = 160;
        radius = 450;
    } else if (size == 1) {
        lift = 140;
        radius = 338;
    } else {
        lift = 120;
        radius = 224;
    }
    angle = ((s->facing + 5) & 7) << 5;
    sprite->pos = *pos;
    sprite->pos.z += lift;
    sprite->pos.x += scale(radius * gUnk_086149C4[(angle + 64) & 255]);
    sprite->pos.y += scale(radius * gUnk_086149C4[angle & 255]);
    s->dx = scale(10 * gUnk_086149C4[(angle + 64) & 255]);
    s->dy = scale(10 * gUnk_086149C4[angle & 255]);
    DirectionalEffect_DecodeAnimationMode(mode, &a, &b);
    Anim_SetSequence(s->animation, s->resource, 1, a, b);
    Player_ShowDirectionalEffect(s);
}
