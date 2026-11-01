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

void Anim_RestartSequence(void *, void *, u16, u8, u8);
void Projectile_SetUpdateCallback(void *, void *);
void Task4E01_StepTimedSequence(void);
s32 Task4E01_StartDirectionalAnimation(u8 *p)
{
    u8 *config = p + 0x18;
    void *resource = ScriptProc_Start(0x922e, 0x32d7);
    u8 direction;
    u8 *frames;
    *(void **)(p + 0x19c) = resource;
    direction = (((u16)((s8)config[6]) + 0x70) >> 5) & 7;
    frames = p + 0x180;
    Anim_RestartSequence(frames, resource, 7, direction, 0);
    StepPlayback((void *)(p + 0x8c), (void *)frames);
    Projectile_SetUpdateCallback(config, Task4E01_StepTimedSequence);
    return 0;
}
