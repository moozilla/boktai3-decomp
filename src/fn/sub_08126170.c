// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "route_sprite_actor.h"
/* Drifting sprite: holds hidden for a delay, then drifts along its angle (sine table), rises, and steps its display sequence; result=2 when it ends. */
#define DriftingSprite_Update sub_08126170
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
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
        asm("" : "+&r"(mask) : "r"(temp));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    ended = 1;
                } else {
                    ended = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
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
