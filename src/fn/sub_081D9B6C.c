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

void Projectile_SetUpdateCallback(void *, void *);
void Projectile_UpdateAnimationTail(void *);

s32 Projectile_UpdateAnimation(u8 *p)
{
    u8 *config = p + 0x18;
    void *first = (void *)(p + 0x8c);
    u32 off = 0x180;
    StepPlayback(first, (void *)(p + off));
    if (*(u32 *)p > 4 || (*(u32 *)p > 2 && (*(u16 *)(p + 0x18) & 0x10))) {
        if (*(u16 *)config & 0x10) {
            u16 *counter = (u16 *)(*(u8 **)(config + 0x54) + 0x50);
            *counter = *counter - 1;
            return 2;
        }
        Projectile_SetUpdateCallback(config, Projectile_UpdateAnimationTail);
        *(u32 *)(p + 0x8c) |= 1;
    }
    return 0;
}
