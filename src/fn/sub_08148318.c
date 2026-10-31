#include "contact_swarm.h"
#include "display_sequence_inline.h"
#include "player.h"
#include "engine.h"
#include "ram.h"
#include "heap.h"
#include "resource_registry.h"
#include "monster_particle_random.h"
// confidence=low: random neighboring directions with short hold durations.
u32 Player_ConsumeStateEntry(struct Player *);
struct WaveringMovementTiming {
    u8 unknown0[0x469];
    u8 directionHold;
};
struct WaveringMovementAnimation {
    u8 pad[0x14C];
    struct ContactSwarmSprite sprite;
    u8 pad2[0x3FC - 0x14C - sizeof(struct ContactSwarmSprite)];
    struct DisplaySequenceState seq;
    const void *resource;
};
s32 Player_GetSmoothedDirection(struct Player *);
void Player_SetDirectionalVelocity(struct Player *, s32);
static inline s32 WaveringMovement_NextSample(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)(RANDOM_TABLE_ADDR + index * 2);
}
// confidence=low: selects nearby directions with randomized hold durations.
static inline void Step_UpdateWaveringMovement(struct ContactSwarmSprite *owner,
                                               struct DisplaySequenceState *state)
{
    const u16 *command;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else {
                state->index--;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    }
}

void Player_UpdateWaveringMovement(struct Player *p)
{
    u8 *timer;
    if (Player_ConsumeStateEntry(p))
        ((struct WaveringMovementTiming *)p)->directionHold = 0;
    p->statusFlags &= ~1;
    timer = &((struct WaveringMovementTiming *)p)->directionHold;
    if ((*timer) == 0) {
        s32 direction = Player_GetSmoothedDirection(p);
        s16 choice;
        if (direction & 1) {
            if (direction > 4) {
                p->heading = (8 - direction) >> 1;
                p->side = 1;
            } else {
                p->heading = direction >> 1;
                p->side = 0;
            }
            choice = WaveringMovement_NextSample() & 7;
            if (choice <= 5) {
                p->facing = direction;
                ((struct WaveringMovementTiming *)p)->directionHold = 9;
            } else if (choice <= 6) {
                p->facing = (direction + 7) & 7;
                ((struct WaveringMovementTiming *)p)->directionHold = 3;
            } else {
                p->facing = (direction + 9) & 7;
                ((struct WaveringMovementTiming *)p)->directionHold = 3;
            }
        } else {
            u32 randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
            u32 sampleOrigin;
            choice = WaveringMovement_NextSample() & 7;
            sampleOrigin = randomOrigin;
            if (choice <= 5) {
                p->facing = direction;
                *timer = 9;
            } else {
                if (choice <= 6)
                    p->facing = (direction + 7) & 7;
                else
                    p->facing = (direction + 9) & 7;
                *timer = 3;
            }
            if (!(p->facing & 1)) {
                s32 sample =
                    ParticleRandom_ReadRelativeSample(sampleOrigin, (const u16 *)RANDOM_TABLE_ADDR);
                s32 facing = p->facing;
                s32 offset = ((sample & 1) << 1) + 7;
                direction = (facing + offset) & 7;
            } else
                direction = p->facing;
            if (direction > 4) {
                p->heading = (8 - direction) >> 1;
                p->side = 1;
            } else {
                {
                    s32 folded = direction >> 1;
                    u8 *heading = &p->heading;
                    u8 side = 0;
                    *heading = folded;
                    p->side = side;
                }
            }
        }
    } else
        --(*timer);
    Player_SetDirectionalVelocity(p, 9);
    Anim_SetSequence(&((struct WaveringMovementAnimation *)p)->seq,
                     ((struct WaveringMovementAnimation *)p)->resource, 0, p->heading, p->side);
    Step_UpdateWaveringMovement(&((struct WaveringMovementAnimation *)p)->sprite,
                                &((struct WaveringMovementAnimation *)p)->seq);
}
