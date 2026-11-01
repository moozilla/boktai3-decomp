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
void Projectile_SetCollisionSize(void *, s32, s32);
u32 sub_08249498(u32, u32);
void Task4E01_BeginImpactAnimation(void *);
void Task4E01_SpawnImpactChildren(void *);
void Task4E01_UpdateExpandingImpact(void *);
// confidence=low: selects the initial impact sequence according to projectile flags.
s32 Task4E01_StartImpactAnimation(u8 *p)
{
    u8 *config = p + 0x18;
    void *resource = ScriptProc_Start(0x922e, 0x32d7);
    *(void **)(p + 0x19c) = resource;
    if (*(u16 *)config & 0x10) {
        Anim_RestartSequence(p + 0x180, resource, 0, 0, 0);
    } else {
        *(u16 *)(p + 0x68) = 3;
        Anim_RestartSequence(p + 0x180, resource, 5, 0, 0);
    }
    StepPlayback((void *)(p + 0x8c), (void *)(p + 0x180));
    *(u32 *)(p + 0x1a0) = 8;
    Projectile_SetUpdateCallback(config, Task4E01_UpdateExpandingImpact);
    return 0;
}
