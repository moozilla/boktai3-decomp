#include "global.h"
#include "area_effect_link.h"
#include "engine_globals.h"
typedef struct Point {
    u16 x, pad2, y, pad6;
} Point;
typedef struct Effect {
    u32 flags;
    u8 pad4[0x14];
    Point pos;
    u8 pad20[8];
    u8 active, ticks, speed, kind;
    s16 dx;
    u8 pad2e[2];
    s16 dy;
    u8 pad32[2];
} Effect;
typedef struct State {
    u8 pad0[0x18];
    s32 participant;
    u8 pad1c[0x9a8 - 0x1c];
    void *graphics;
    u8 active, next, ticks, pad9af;
    Effect effects[4];
} State;
extern const s16 gUnk_086149C4[];
extern u8 gUnk_0203B400[];
void SpriteDrawable_SelectTileFrame(Effect *, void *, s32);
static inline s32 random_value(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(u16 *)((u8 *)RANDOM_TABLE_ADDR + (gRandomIndex << 1));
}
static inline s32 random_first(void)
{
    u8 *table = gUnk_0203B400;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(u16 *)(table + (gRandomIndex << 1));
}
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline u16 linkValue(s32 index)
{
    struct AreaEffectLinkState *link = gUnk_020004B4;
    if (!link || index < 0)
        return 0;
    return link->playerValues[index];
}
// The final draw reads the table through RANDOM_TABLE_ADDR: a literal address
// changes reload register rotation (r2 instead of r1), as in the original.
// confidence=low: spawns a converging particle at a rate scaled by the link player's value.
void sub_082392E8(State *s, Point *pos)
{
    u32 inc = linkValue(s->participant);
    s->ticks += inc + 2;
    if (s->ticks >= 40) {
        Effect *e = &s->effects[s->next];
        Effect *draw = e;
        s32 angle;
        s32 radius;
        s32 mask = 255;
        draw->flags &= ~1;
        e->kind = 2;
        SpriteDrawable_SelectTileFrame(draw, s->graphics, 2);
        angle = random_first() & mask;
        do {
            radius = (random_first() & 63) + 128;
        } while (0);
        do {
            e->dx = scale(radius * gUnk_086149C4[(angle + 64) & mask]);
        } while (0);
        e->dy = scale(radius * gUnk_086149C4[angle]);
        draw->pos = *pos;
        draw->pos.x += (u16)e->dx;
        draw->pos.y += (u16)e->dy;
        e->speed = (random_value() & 3) + 14;
        e->ticks = 0;
        e->active = 1;
        if ((++s->next) > 3) {
            s->next = 0;
        }
        s->ticks -= 40;
        s->active = 1;
    }
}
