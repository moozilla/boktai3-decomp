// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"
#include "engine.h"
#include "ram.h"
struct TileTriggerManager {
    u8 unknown0[0x18];
    void *animationData;
};
struct TileTrigger {
    u32 flags;
    u8 unknown4[0xC];
    u16 spriteFrame;
    u8 unknown12[0x62];
    struct DisplaySequenceState animation;
    struct SceneVec position;
    u8 unknown8C[0x2C];
    u16 sequence, delay, unknownBC;
    s16 state;
    u16 triggerFlags;
    u8 forced, frame;
    s32 timer;
};
void TileTrigger_ApplyContactEffects(struct TileTrigger *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void TileTrigger_EmitParticles(struct TileTrigger *);
static inline u32 StepPlayback(struct TileTrigger *owner, struct DisplaySequenceState *state,
                               struct TileTrigger *base, u32 one)
{
    struct TileTrigger *sprite = owner;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->animation.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->spriteFrame = *command >> 6;
    {
        u32 input = state->flags;
        u32 first;
        bit = one;
        first = bit;
        asm("" : "+r"(first));
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
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
            owner->flags |= 8;
        else
            owner->flags &= ~8;
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
// confidence=low: restarts the trigger animation at frame 1, steps it once and enters state 6.
void sub_08107D54(struct TileTriggerManager *manager, struct TileTrigger *p)
{
    p->flags &= ~1;
    Anim_SetSequence(&p->animation, manager->animationData, 0, (u8)p->sequence, 0);
    p->animation.index = 1;
    StepPlayback(p, &p->animation, p, p->animation.index);
    p->state = 6;
    p->triggerFlags = (p->triggerFlags & ~0x20) | 2;
}
