#include "global.h"
#include "player/cutscene/player_cue_sequence.h"
#include "bosses/nidhoggr/nidhoggr_effect_motion_layout.h"
#include "bosses/nidhoggr/nidhoggr_pool_sprite_layout.h"

struct NidhoggrAnimatedSlot {
    void (*update)(struct NidhoggrAnimatedSlot *);
    void (*onComplete)(struct NidhoggrAnimatedSlot *);
    u8 animationData[0x1c];
    struct NidhoggrPoolSprite sprite;
    struct DisplaySequenceState sequence;
    struct NidhoggrEffectMotion motion;
    u8 lifetime;
    u8 unknown69[3];
};
s32 Callback_InvokeActor(void *, void *);
static inline void StepSequence(struct NidhoggrAnimatedSlot *owner)
{
    PlayerCue_StepSequence(&owner->sprite, &owner->sequence);
}

void NidhoggrEffect_TickSpriteLifetime(struct NidhoggrAnimatedSlot *ownerArg)
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
