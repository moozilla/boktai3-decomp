#include "global.h"
/* Tentative screen-space particle family; coordinates are packed signed halves. */
union ScreenMotePosition {
    struct {
        s16 x, y, z, pad;
    } axis;
    u32 words[2];
};
struct ScreenMoteSprite {
    u32 flags;
    u8 unknown4[0x14];
    union ScreenMotePosition position;
    u8 tail[8];
};
struct ScreenMoteState {
    union ScreenMotePosition position;
    u8 unknown8[8];
    union ScreenMotePosition *target;
    u8 unknown14[2], phase;
    s8 spriteIndex;
    u16 age, unknown1A, remaining;
    u8 delay, entering;
};
struct ScreenMoteManager {
    u8 unknown0[0x18];
    void *resource;
    struct ScreenMoteSprite sprites[1];
};
s32 Div(s32, s32);
void SpriteDrawable_SelectTileFrame(struct ScreenMoteSprite *, void *, u32);
void ScreenMote_ApproachTarget(struct ScreenMoteManager *m, struct ScreenMoteState *s)
{
    struct ScreenMoteSprite *sprite = &m->sprites[s->spriteIndex];
    union ScreenMotePosition target;
    if (s->entering == 1) {
        sprite->flags &= ~1;
        s->remaining = 12;
        s->delay = 0;
        s->entering = 0;
    }
    target = *s->target;
    target.axis.x += 16;
    target.axis.y -= 12;
    s->position.axis.x += Div(target.axis.x - s->position.axis.x, s->remaining);
    s->position.axis.y += Div(target.axis.y - s->position.axis.y, s->remaining);
    s->remaining--;
    sprite->position = s->position;
    if (++s->delay > 3)
        SpriteDrawable_SelectTileFrame(sprite, m->resource, 3);
    if (s->remaining == 0)
        s->phase = 2;
    s->age++;
}
