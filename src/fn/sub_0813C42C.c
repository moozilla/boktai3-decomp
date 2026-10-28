#include "global.h"
#include "engine_globals.h"
// MATCHING_CONCESSION: register-pinning
// Cleanup: angle and RNG temporaries use local register hints.
/* Tentative role: periodically spawn an inward-moving effect in a four-slot ring. */
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
    u8 pad0[0x9a8];
    void *graphics;
    u8 active, next, ticks, pad9af;
    Effect effects[4];
} State;
extern const s16 gUnk_086149C4[];
extern u8 gUnk_0203B400[];
extern u8 *gGameState;
void SpriteDrawable_SelectTileFrame(Effect *, void *, s32);
static inline s32 random_value(void)
{
    register u8 *table asm("r2");
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex << 1;
    table = gUnk_0203B400;
    offset += (u32)table;
    return *(u16 *)offset;
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
void ConvergingEffects_EmitNearParticle(State *s, Point *pos)
{
    s32 step = *(s16 *)(gGameState + 0x876);
    s->ticks = step + 2 + s->ticks;
    if (s->ticks > 39) {
        Effect *e = &s->effects[s->next];
        Effect *draw = e;
        register s32 angle asm("r12");
        s32 radius;
        draw->flags &= ~1;
        e->kind = 2;
        SpriteDrawable_SelectTileFrame(draw, s->graphics, 2);
        angle = random_first() & 255;
        radius = (random_first() & 63) + 128;
        e->dx = scale(radius * gUnk_086149C4[(angle + 64) & 255]);
        e->dy = scale(radius * gUnk_086149C4[angle]);
        draw->pos = *pos;
        draw->pos.x += (u16)e->dx;
        draw->pos.y += (u16)e->dy;
        e->speed = (random_value() & 3) + 14;
        e->ticks = 0;
        e->active = 1;
        if (++s->next > 3)
            s->next = 0;
        s->ticks -= 40;
        s->active = 1;
    }
}
