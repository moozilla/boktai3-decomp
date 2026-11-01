#include "global.h"
#include "player_cue_sequence.h"

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

#define ScenePanelEffect_TickSpriteLifetime sub_0818F0FC
void ScenePanelEffect_TickSpriteLifetime(struct NidhoggrAnimatedSlot *ownerArg)
{
    struct NidhoggrAnimatedSlot *owner = ownerArg;
    StepSequence(owner);
    if (--owner->lifetime == 0) {
        owner->update = 0;
        if (owner->onComplete) {
            Callback_InvokeActor(owner, owner->onComplete);
            owner->onComplete = 0;
        }
    }
}
