// MATCHING_CONCESSION: register-pinning
#include "global.h"

#include "contact_swarm.h"
#include "ram.h"

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
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
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

void ActorDisplay_UpdateScreenAnchor(void *);
void ActorDisplay_UpdateForegroundSprite(void *);
void ActorDisplay_UpdateAccentSprite(void *);
void ActorMotion_AdvanceTowardScreenTarget(void *, s32);
void SceneActors_EmitPeriodicParticles(void *, void *);
extern const u16 gUnk_086141BC[];
// confidence=low: called by SceneActors_EnterMovementState.
void SceneActors_UpdateMovingDisplay(u8 *input, void *particles)
{
    u8 *p = input;
    void *savedParticles;
    u8 *context;
    u8 *enter;
    savedParticles = particles;
    context = p;
    enter = p + 0xB1;
    if (*enter) {
        struct Drawable *foreground;
        struct Drawable *accent;
        struct DisplaySequenceState *state;
        u32 zero;
        zero = 0;
        *enter = zero;
        foreground = (struct Drawable *)(p + 0x2C0);
        accent = (struct Drawable *)(p + 0x308);
        *(u32 *)(p + 0x298) = zero;
        *(u32 *)(p + 0x290) = zero;
        foreground->unknown3C = zero;
        state = (struct DisplaySequenceState *)(p + 0x2EC);
        Anim_SetSequence(state, *(void **)(p + 0x390), zero, 0, zero);
        StepPlayback(foreground, state);
        accent->unknown3C = 4;
        state = &accent->sequence;
        Anim_SetSequence(state, *(void **)(context + 0x390), 4, 0, 0);
        StepPlayback(accent, state);
    }
    ActorMotion_AdvanceTowardScreenTarget(p, *(s32 *)(context + 0x294));
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    {
        u32 value = gUnk_086141BC[gEffectRandomTableIndex];
        u32 *offset = (u32 *)(context + 0x288);
        *offset = value & 1;
    }
    ActorDisplay_UpdateScreenAnchor(p);
    ActorDisplay_UpdateForegroundSprite(p);
    ActorDisplay_UpdateAccentSprite(p);
    SceneActors_EmitPeriodicParticles(p, savedParticles);
}
