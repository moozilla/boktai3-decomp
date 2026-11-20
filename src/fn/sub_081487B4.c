#include "player_cue.h"
#include "player.h"
#include "player_cue_sequence.h"
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
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.setupResource, 1, p->heading,
                     p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &p->cueCollider.sequence);
    if (!p->movementModifierActive)
        ActorSequence_Begin(p, 0, 0);
}
