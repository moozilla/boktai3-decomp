#include "global.h"
#include "engine_globals.h"
#include "ram.h"
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

struct Effect {
    struct Sprite sprite;
    struct Anim anim;
    u8 pad58[0xA8];
    struct Vec *target;
    u8 pad104[0x68];
    struct Vec velocity;
    u16 unknown174;
    u8 state;
    s8 slot;
    u16 frame;
    u8 entered, speed, ended, heading;
    u16 unknown17E, lifetime, riseTimer, delay, turnTimer, maxSpeed;
};
struct Pool {
    u8 pad[0x18];
    void *resource;
    u32 allocated;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void LinkedEffect_UpdateColliders(struct Effect *);
static inline u32 RandomValue(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return gRandomTable[index];
}
static inline s32 Truncate(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
void Collider_Remove(void *);
// confidence=low: terminal animation releases the linked effect slot and collider.
void LinkedEffect_FinishRemoval(struct Pool *pool, struct Effect *p)
{
    if (p->entered) {
        Anim_SetSequence(&p->anim, pool->resource, 24, 0, 0);
        p->entered = 0;
    }
    if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)&p->sprite,
                                    (struct DisplaySequenceState *)&p->anim)) {
        pool->allocated &= ~(1 << p->slot);
        p->state = 0;
        if (p->sprite.visible)
            Sprite_Remove(p);
        Collider_Remove((u8 *)p + 0xac);
        p->slot = -1;
        p->unknown174 = 0;
    }
}
