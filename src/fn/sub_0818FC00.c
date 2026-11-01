#define NIDHOGGR_MASK "r6"
#include "global.h"
#include "display_animation.h"
#include "player_cue_sequence.h"
struct NidhoggrVector {
    s16 x, y, z, pad;
};
struct NidhoggrSprite {
    u32 flags;
    u8 pad4[12];
    u16 frame;
    u8 pad12[10];
    struct NidhoggrVector position;
    u8 pad24[8];
};
struct NidhoggrEffect {
    void *update, *destroy;
    u8 animation[0x1c];
    struct NidhoggrSprite sprite;
    struct DisplaySequenceState sequence;
    u8 pad60[0x114];
    void *anchor;
    struct NidhoggrVector velocity;
    u8 state, age;
    s16 ground;
    u8 *boss;
};
#ifndef NIDHOGGR_MASK
#define NIDHOGGR_MASK "r5"
#endif
static inline u32 Nidhoggr_StepAnimation(struct NidhoggrEffect *owner)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, &owner->sequence);
}

struct EffectCollider {
    u8 pad[6];
    u16 flags;
};
void Nidhoggr_UpdateDetachedAttackEffect(struct NidhoggrEffect *p)
{
    u32 ended = Nidhoggr_StepAnimation(p);
    u32 offset = 0x180;
    u8 *state;
    u32 old;
    state = (u8 *)p + offset;
    old = *state;
    switch (old) {
        case 0:
            if (ended) {
                struct EffectCollider *collider;
                u32 mask;
                p->anchor = 0;
                p->velocity.x = 12;
                p->velocity.y = old;
                p->velocity.z = old;
                p->ground = p->sprite.position.x;
                (*state)++;
                collider = (void *)((u8 *)p + 0xcc);
                mask = ~4;
                collider->flags &= mask;
            }
            break;
        case 1:
            if (p->sprite.position.x - p->ground > 0x1000) {
                struct EffectCollider *collider;
                p->update = 0;
                if (p->destroy) {
                    Callback_InvokeActor(p, p->destroy);
                    p->destroy = 0;
                }
                collider = (void *)((u8 *)p + 0xcc);
                collider->flags |= 4;
            }
            break;
    }
}
