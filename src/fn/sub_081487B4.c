#include "contact_swarm.h"
#include "player.h"
#include "player_cue_sequence.h"
struct ReleaseAnimation {
    u8 pad[0x14C];
    struct ContactSwarmSprite sprite;
    u8 pad2[0x3FC - 0x14C - sizeof(struct ContactSwarmSprite)];
    struct DisplaySequenceState seq;
    const void *resource;
};
u32 Player_ConsumeStateEntry(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
// confidence=low: drains the action timer and waits for the movement animation gate.
void Player_UpdateTimedMovementRecovery(struct Player *p)
{
    u8 *timer;
    Player_ConsumeStateEntry(p);
    timer = (u8 *)p + 0x466;
    if (*timer > 59)
        *timer -= 60;
    else {
        p->statusFlags &= ~1;
        *timer = 0;
    }
    Anim_SetSequence(&((struct ReleaseAnimation *)p)->seq, *(const void **)((u8 *)p + 0x40C), 1,
                     p->heading, p->side);
    PlayerCue_StepSequence(&((struct ReleaseAnimation *)p)->sprite,
                           &((struct ReleaseAnimation *)p)->seq);
    {
        u32 offset = 0x5BB;
        u8 *gate;
        gate = (u8 *)p + offset;
        if (!*gate)
            ActorSequence_Begin(p, 0, 0);
    }
}
