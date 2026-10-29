// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
// confidence=low: moves two sprite sequences left before destruction.
#include "scene_vec.h"
#include "ram.h"
s32 SceneMover_StepPastScreenX(void *, s32, s32);
extern const u16 gUnk_086141BC[];
struct Drawable {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    u32 unknown3C;
    s32 ended;
    u32 mode;
};
void ActorDisplay_UpdateScreenAnchor(void *);
void ActorDisplay_UpdateForegroundSprite(void *);
void ActorDisplay_UpdateAccentSprite(void *);
void SceneActors_EmitPeriodicParticles(void *, s32);
void SceneActors_UpdateIdleDisplay(void *);
// confidence=low: departs past the left edge, then schedules destruction.
#define SceneActors_UpdatePastEdgeDeparture sub_081EDE4C
/* Tentative fields of the actor's departure display state. */
struct DepartureActor {
    u8 unknown0[0x90];
    s32 stateTimer;
    u8 unknown94[0xE];
    u8 phase, unknownA3;
    s32 phaseTimer;
    u8 unknownA8[9], entered;
    u8 unknownB2[0x4E];
    void (*callback)(void *);
    u8 unknown104[0x180];
    s32 verticalVelocity, randomOffset;
    u8 unknown28C[4];
    u32 field290;
    u8 unknown294[4];
    u32 field298;
    u8 unknown29C[0x24];
    struct Drawable foreground, accent;
    u8 unknown350[0x40];
    const void *sequenceResource;
};

static inline void StepPlayback(struct Drawable *owner, struct DisplaySequenceState *state)
{
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command;
    u32 duration;
    u32 flags;
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
        u32 bit = maskBit;
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

                } else {
                    state->index = index - 1;
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

                } else {
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
        }
    }
}

static inline u32 RandomValue(void)
{
    return gUnk_086141BC[gEffectRandomTableIndex];
}

void SceneActors_UpdatePastEdgeDeparture(struct DepartureActor *input, s32 time)
{
    s32 result = 0;
    struct DepartureActor *p = input;
    u8 *enter;
    enter = &p->entered;
    if (*enter) {
        struct Drawable *foreground;
        struct Drawable *accent;
        struct DisplaySequenceState *state;
        *enter = result;
        foreground = &p->foreground;
        accent = &p->accent;
        p->field298 = result;
        p->field290 = result;
        foreground->unknown3C = result;
        state = &p->foreground.sequence;
        Anim_SetSequence(state, p->sequenceResource, 0, 0, result);
        StepPlayback(foreground, state);
        accent->unknown3C = 4;
        state = &accent->sequence;
        Anim_SetSequence(state, p->sequenceResource, 4, 0, 0);
        StepPlayback(accent, state);
    }
    result = SceneMover_StepPastScreenX(input, -40, 0x5000);
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    {
        u32 one;
        p->randomOffset = RandomValue() & (one = 1);
        ActorDisplay_UpdateScreenAnchor(input);
        ActorDisplay_UpdateForegroundSprite(input);
        ActorDisplay_UpdateAccentSprite(input);
        SceneActors_EmitPeriodicParticles(input, time);
        if (result) {
            {
                void (*callback)(void *) = SceneActors_UpdateIdleDisplay;
                input->callback = callback;
            }
            input->stateTimer = 0;
            input->entered = one;
            input->phase = 0;
            input->phaseTimer = 0;
            p->verticalVelocity = -16;
        }
    }
}

void sub_081EE12C(void)
{
}
