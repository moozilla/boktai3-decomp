#include "global.h"
#include "player_cue_sequence.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u32 flags;
    u8 visible, unknown5, palette, unknown7;
    s8 scaleX, scaleY;
    u8 padA[6];
    u16 frame;
    u8 pad12[10];
    struct Vec position;
    u8 pad24[0x24];
};
struct Anim {
    const volatile u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct TrailSprite {
    struct Sprite sprite;
    struct Anim anim;
    u16 lifetime, unknown5A;
};
struct Effect {
    u8 unknown0[6];
    u8 palette;
    u8 unknown7[0x15];
    struct Vec position;
    u8 unknown24[0x18];
    struct TrailSprite trail[2];
    u8 unknownF4[0xB6];
    u8 next, interval;
    void *resource;
    u8 unknown1B0[8];
    u16 frame;
};
s32 Mod(s32, s32);
// confidence=low: rotates two short-lived copies of the parent sprite as a trail.
void TwinSpriteEffect_EmitTrail(struct Effect *p)
{
    if (!Mod(p->frame, p->interval)) {
        Anim_SetSequence((void *)&p->trail[p->next].anim, p->resource, 1, 0, 0);
        PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->trail[p->next].sprite,
                               (struct DisplaySequenceState *)&p->trail[p->next].anim);
        p->trail[p->next].sprite.position = p->position;
        p->trail[p->next].sprite.flags &= ~1;
        p->trail[p->next].sprite.flags |= 2;
        p->trail[p->next].sprite.palette = p->palette;
        p->trail[p->next].lifetime = 4;
        if (++p->next > 1)
            p->next = 0;
    }
}
