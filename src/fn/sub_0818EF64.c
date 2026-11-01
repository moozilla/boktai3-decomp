#include "global.h"
#include "player_cue_sequence.h"

// Exact with register-pinning and empty-asm concessions.
struct NidhoggrPoolSequence {
    const volatile u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct NidhoggrEffectMotion {
    u16 x, y, z, unknown6;
};
struct NidhoggrPoolSprite {
    u32 flags;
    u8 unknown4[12];
    u16 frame;
    u8 unknown12[10];
    struct NidhoggrEffectMotion position;
    u8 unknown24[8];
};
struct NidhoggrAnimatedSlot {
    void (*update)(struct NidhoggrAnimatedSlot *);
    void (*onComplete)(struct NidhoggrAnimatedSlot *);
    u8 animationData[0x1c];
    struct NidhoggrPoolSprite sprite;
    struct NidhoggrPoolSequence sequence;
    struct NidhoggrEffectMotion motion;
    u8 lifetime;
    u8 unknown69[3];
};
s32 Callback_InvokeActor(void *, void *);
static inline void StepSequence(struct NidhoggrAnimatedSlot *owner)
{
    PlayerCue_StepSequence(&owner->sprite, &owner->sequence);
}

struct EffectDelta {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline void AddMotion(struct NidhoggrEffectMotion *motion,
                             const struct NidhoggrEffectMotion *delta)
{
    motion->x += delta->x;
    motion->y += delta->y;
    motion->z += delta->z;
}

void Nidhoggr_StepMovingSpriteEffect(struct NidhoggrAnimatedSlot *ownerArg)
{
    struct NidhoggrAnimatedSlot *owner = ownerArg;
    struct EffectDelta delta;
    struct NidhoggrEffectMotion *motion;
    struct NidhoggrEffectMotion *position;
    StepSequence(owner);
    position = &owner->sprite.position;
    motion = &owner->motion;
    position->x = position->x + motion->x;
    position->y = position->y + motion->y;
    position->z = position->z + motion->z;
    delta.x = -(((s16)motion->x) >> 4);
    delta.y = -(((s16)owner->motion.y) >> 4) - 8;
    delta.z = -(((s16)owner->motion.z) >> 4);
    AddMotion(motion, (struct NidhoggrEffectMotion *)&delta);
    if ((--owner->lifetime) == 0) {
        owner->update = 0;
        if (owner->onComplete) {
            Callback_InvokeActor(owner, owner->onComplete);
            owner->onComplete = 0;
        }
    }
}
