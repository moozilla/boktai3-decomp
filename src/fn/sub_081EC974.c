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
struct ActorDisplayState {
    u8 unknown0[0x278];
    u16 screenX;
    u8 unknown27A[0x308 - 0x27A];
    struct Drawable accent;
    u8 unknown350[0x5E0 - 0x350];
    u16 origin;
    u8 unknown5E2[10];
    s16 hidden;
};

void ActorDisplay_UpdateAccentSprite(struct ActorDisplayState *actor)
{
    struct Drawable *draw = &actor->accent;
    u16 *origin;
    u32 zero;
    u16 *screen = &actor->screenX;
    s32 offset = draw->ended + 38;
    u16 x = *screen;
    origin = &actor->origin;
    x = x - offset + *origin;
    zero = 0;
    draw->sprite.position.x = x;
    draw->sprite.position.y = *origin + 149;
    if (!actor->hidden)
        draw->sprite.flags &= ~1;
    if (++draw->ended > 2)
        draw->ended = zero;
    if (draw->mode == 0) {
        if (StepPlayback(draw) == 1)
            draw->ended = 0;
    } else
        draw->ended = zero;
}
