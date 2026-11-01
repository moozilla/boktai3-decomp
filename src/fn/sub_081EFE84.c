#include "global.h"

#include "contact_swarm.h"
#include "player_cue_sequence.h"

// Drawable layout shared with ActorDisplay_UpdateForegroundSprite.
struct Drawable {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    u32 unknown3C;
    s32 ended;
    u32 mode;
};
static inline u32 StepPlayback(struct Drawable *owner, struct DisplaySequenceState *state)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, state);
}

static inline u32 StepPlaybackOne(struct Drawable *owner, struct DisplaySequenceState *state,
                                  u32 maskBit)
{
    return PlayerCue_StepSequenceEndedWithFirstFlipMask(&owner->sprite, state, maskBit);
}

static inline u8 ConsumeEntry(u8 *p)
{
    u8 *q = p + 0xB0;
    if (*q) {
        *q = 0;
        *(q - 1) = 0;
        return TRUE;
    }
    return FALSE;
}

void SingletonSlot7_UpdateTransitionDisplay(u8 *p)
{
    u8 *context = p;
    u32 flag = ConsumeEntry(context);
    if (flag == 1) {
        struct Drawable *foreground = (struct Drawable *)(p + 0x2C0);
        struct DisplaySequenceState *state;
        foreground->unknown3C = 2;
        state = (struct DisplaySequenceState *)(p + 0x2EC);
        Anim_SetSequence(state, *(void **)(p + 0x390), 2, 0, 0);
        StepPlaybackOne(foreground, state, flag);
    }
    {
        if (*(u32 *)(context + 0xC4) == 0x78) {
            struct Drawable *foreground;
            struct DisplaySequenceState *state;
            *(u32 *)(p + 0x2FC) = 0;
            state = (struct DisplaySequenceState *)(p + 0x2EC);
            Anim_SetSequence(state, *(void **)(p + 0x390), 0, 0, 0);
            foreground = (struct Drawable *)(p + 0x2C0);
            StepPlayback(foreground, state);
            Trigger_ConsumePending(context + 0x118, 1);
        }
        (*(u32 *)(context + 0xC4))++;
    }
}
