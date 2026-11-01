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

struct Pos8 {
    u32 a, b;
};
struct Sub {
    u8 pad[0x1c];
    struct Pos8 pos;
};
void Anim_RestartSequence(void *, void *, u16, u8, u8);
void Projectile_SetUpdateCallback(void *, void *);
void Projectile_SetCollisionSize(void *, s32, s32, s32);
void Task4E01_UpdateGrowingImpact(void);
// confidence=low: initializes a small collision, starts its script and first animation frame.
s32 Task4E01_StartTimedImpact(u8 *p)
{
    u8 *config = p + 0x18;
    void *resource;
    u8 *frames;
    *(u32 *)(p + 0x1a0) = 8;
    Projectile_SetCollisionSize(config, 8, 0x50, 8);
    ((struct Sub *)(p + 0xb8))->pos = *(struct Pos8 *)(p + 0x40);
    resource = ScriptProc_Start(0x922e, 0x32d7);
    *(void **)(p + 0x19c) = resource;
    frames = p + 0x180;
    Anim_RestartSequence(frames, resource, 3, 0, 0);
    StepPlayback((void *)(p + 0x8c), (void *)frames);
    Projectile_SetUpdateCallback(config, Task4E01_UpdateGrowingImpact);
    return 0;
}
