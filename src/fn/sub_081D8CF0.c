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
void Projectile_SetCollisionSize(void *, s32, s32, s32);
// confidence=low: grow an impact and switch its animation after playback ends.
#define Task4E01_UpdateGrowingImpact sub_081D8CF0
s32 Task4E01_UpdateGrowingImpact(u8 *p)
{
    u8 *config = p + 0x18;
    u8 ended;
    if ((*((u32 *)p)) <= 11) {
        *((u32 *)(p + 0x1A0)) += 10;
        Projectile_SetCollisionSize(config, *((u32 *)(p + 0x1A0)), 0x50, *((u32 *)(p + 0x1A0)));
        ((struct Sub *)(p + 0xb8))->pos = *(struct Pos8 *)(p + 0x40);
    }
    ended = StepPlayback((void *)(p + 0x8C), (void *)(p + 0x180));
    if (ended == 1 && !(*((u32 *)(p + 0x190)))) {
        Anim_RestartSequence(p + 0x180, *((void **)(p + 0x19C)), 8, 0, 0);
        StepPlayback((void *)(p + 0x8C), (void *)(p + 0x180));
        ++(*((u32 *)(p + 0x190)));
    }
    {
        u8 k = 2;
        *((u32 *)(p + 4)) |= k;
    }
    if ((*((u32 *)p)) >= (*((u16 *)(config + 2))))
        return 2;
    return 0;
}
