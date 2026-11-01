// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "route_sprite_actor.h"
#include "player_cue_sequence.h"
/* Drifting sprite: holds hidden for a delay, then drifts along its angle (sine table), rises, and steps its display sequence; result=2 when it ends. */
void Anim_SetDurationScale(struct DisplaySequenceState *, u32);
struct DriftingSprite {
    struct ContactSwarmSprite sprite;
    u8 graphics[0x1C];
    struct DisplaySequenceState sequence;
    u8 result;
    u8 unknown59;
    u8 angle;
    u8 unknown5B;
    u16 rise;
    s16 speed;
    u8 unknown60[4];
    u16 delay;
    u8 unknown66[2];
    u16 durationScale;
    u8 unknown6A[3];
    u8 riseSteps;
    u16 hidden;
};
static inline u32 StepPlayback(struct RouteSpriteActor *owner, struct DisplaySequenceState *state,
                               struct RouteSpriteActor *base)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, &base->sequence);
}
void DriftingSprite_Update(struct DriftingSprite *s)
{
    struct DriftingSprite *self = s;
    struct DisplaySequenceState *sequence;
    u16 *counter;
    counter = &s->hidden;
    if (*counter) {
        s->sprite.flags |= 1;
        asm("" ::: "memory"); /* the original reloads the counter after the flag store */
        (*counter)--;
        return;
    }
    s->sprite.flags &= ~1;
    sequence = &s->sequence;
    Anim_SetDurationScale(sequence, s->durationScale);
    {
        u8 *angle;
        s16 *speed;
        speed = &s->speed;
        s->sprite.position.x +=
            (*speed * gUnk_086149C4[(*(angle = &s->angle) + 0x40) & 0xFF]) >> 12;
        s->sprite.position.z += (*speed * gUnk_086149C4[*angle]) >> 12;
    }
    counter = &s->delay;
    if (*counter)
        (*counter)--;
    else {
        s->sprite.position.y += s->rise;
        counter = (u16 *)&s->riseSteps;
        if (*(u8 *)counter)
            (*(u8 *)counter)--;
        else
            s->sprite.priority = 2;
    }
    if (StepPlayback((struct RouteSpriteActor *)self, sequence, (struct RouteSpriteActor *)s))
        s->result = 2;
}
