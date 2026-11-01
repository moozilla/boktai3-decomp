// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: accent role inferred from cycling offset and paired foreground refresh.
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
static inline u32 StepPlayback(struct Drawable *owner)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, &owner->sequence);
}
void ActorDisplay_UpdateAccentSprite(u8 *p)
{
    struct Drawable *draw = (struct Drawable *)(p + 0x308);
    u16 *origin;
    u32 zero;
    u16 *screen = (u16 *)(p + 0x278);
    s32 offset = draw->ended + 38;
    register s32 x asm("r0") = *screen;
    origin = (u16 *)(p + 0x5E0);
    x -= offset;
    x += *origin;
    zero = 0;
    draw->sprite.position.x = x;
    draw->sprite.position.y = *origin + 149;
    if (!*(s16 *)(p + 0x5EC))
        draw->sprite.flags &= ~1;
    if (++draw->ended > 2)
        draw->ended = zero;
    if (draw->mode == 0) {
        if (StepPlayback(draw) == 1)
            draw->ended = 0;
    } else
        draw->ended = zero;
}
