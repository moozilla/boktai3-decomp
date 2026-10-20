#include "global.h"
struct TimedSpriteConfig {
    u16 flags;
    u16 duration;
};
struct TimedSpriteSprite {
    u32 flags;
    u8 unknown4[0xC];
    u16 frame;
};
struct TimedSprite {
    u32 tick;
    u32 flags;
    u8 unknown8[0x10];
    struct TimedSpriteConfig config;
    u8 unknown1C[0x8C - 0x1C];
    struct TimedSpriteSprite sprite;
    u8 unknownA0[0x1A4 - 0xA0];
    s16 timer;
};
s32 Task4E01_UpdateTimedSprite(struct TimedSprite *p)
{
    struct TimedSpriteConfig *config = (struct TimedSpriteConfig *)((u8 *)p + 0x18);
    struct TimedSpriteSprite *sprite = &p->sprite;
    s32 t;
    sprite->flags &= ~1;
    if (p->tick & 1) {
        t = p->timer;
        sprite->frame = t + 0x19;
    } else if (p->timer == 0) {
        sprite->flags |= 1;
    } else {
        t = p->timer;
        sprite->frame = t + 0x18;
    }
    {
        u32 two = 2;
        p->flags |= two;
    }
    if (p->tick >= config->duration)
        return 2;
    return 0;
}
