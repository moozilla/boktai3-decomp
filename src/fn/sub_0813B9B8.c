#include "global.h"
struct HiddenEffectPosition { u32 words[2]; };
struct HiddenEffectSprite {
    u32 flags;
    u8 unknown4[0x14];
    struct HiddenEffectPosition position;
    u8 unknown20[8];
};
struct HiddenEffectOwner {
    u8 unknown0[0x744];
    void *graphics;
    struct HiddenEffectSprite sprite;
    u8 active, ticks, animation;
};
void sub_08217EEC(struct HiddenEffectSprite *, void *, u32);
void HiddenEffect_StartAtPosition(struct HiddenEffectOwner *s, struct HiddenEffectPosition *position, s32 alternate)
{
    struct HiddenEffectSprite *sprite = &s->sprite;
    if (alternate) s->animation = 0x48;
    else s->animation = 4;
    sub_08217EEC(sprite, s->graphics, s->animation);
    sprite->position = *position;
    sprite->flags &= ~1;
    s->active = 1;
    s->ticks = 0;
}
