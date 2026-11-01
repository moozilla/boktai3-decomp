#include "global.h"
#include "display_animation.h"
#include "player_cue_sequence.h"

struct SunDiscSprite {
    u32 flags;
    u8 unk4[0xc];
    u16 frame;
    u8 unk12[0x1a];
    struct DisplaySequenceState sequence;
    u8 unk3C[0xbc];
    u8 resetPending;
};

s32 BlinkingSprite_UpdateExpiry(void *, void *);

void SunDiscSprite_Update(void *pool, struct SunDiscSprite *sprite)
{
    if (sprite->resetPending)
        sprite->resetPending = 0;
    PlayerCue_StepSequence((struct ContactSwarmSprite *)sprite, &sprite->sequence);
    BlinkingSprite_UpdateExpiry(pool, sprite);
}
