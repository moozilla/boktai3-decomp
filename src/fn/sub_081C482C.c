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

void Sound_PlaySong(u32);

struct SlidePanel {
    u8 pad0[0x18];
    struct ContactSwarmSprite sprite;
    u8 graphics[0x1C];
    struct DisplaySequenceState sequence;
    void *resource;
};
void ScenePanelSubject_SlideOpen(struct SlidePanel *owner)
{
    owner->sprite.position.x -= 0xD2;
    owner->sprite.position.z += 0xD2;
    Anim_SetSequence(&owner->sequence, owner->resource, 1, 0, 0);
    StepPlayback((void *)&owner->sprite, (void *)&owner->sequence);
    Sound_PlaySong(0x50E);
}
