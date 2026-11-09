#include "global.h"
#include "display_sequence_sprite.h"

struct NidhoggrAnimatedSlot {
    void *update, *onComplete;
    u8 unknown8[0x1c];
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState sequence;
};
s32 Callback_InvokeActor(void *, void *);

void Nidhoggr_StepPoolAnimation(struct NidhoggrAnimatedSlot *owner)
{
    if (DisplaySequence_StepSprite(&owner->sprite, &owner->sequence)) {
        owner->update = 0;
        if (owner->onComplete) {
            Callback_InvokeActor(owner, owner->onComplete);
            owner->onComplete = 0;
        }
    }
}
