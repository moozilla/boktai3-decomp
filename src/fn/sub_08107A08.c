// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"
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
static inline u32 StepPlayback(struct TileTrigger *owner, struct DisplaySequenceState *state,
                               struct TileTrigger *base)
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
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
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
// confidence=low: runs the trigger animation once the countdown passes 30 and advances the state when it ends.
void TileTrigger_StepActivation(struct TileTrigger *p)
{
    struct TileTrigger *self = p;
    struct TileTriggerManager *manager = gTileTriggerManager;
    if (p->timer > 30) {
        u8 *forced = &p->forced;
        if (*forced) {
            Anim_RestartSequence(&p->animation, manager->animationData, 0, (u8)p->sequence, 4);
            *forced = 0;
            switch (p->sequence) {
                case 0:
                case 1:
                    Sound_PlaySong(0x1AC);
                    break;
            }
        }
        if (!StepPlayback(self, &p->animation, p))
            goto tick;
        self->flags |= 1;
        p->state = 1;
        p->timer = 0;
        return;
    } else if (p->animation.index == 1) {
        TileTrigger_ApplyContactEffects(p);
    }
tick:
    p->timer++;
}

void TileTrigger_StepRelease(struct TileTrigger *p)
{
    struct TileTrigger *self;
    struct TileTriggerManager *manager;
    self = p;
    manager = gTileTriggerManager;
    p->flags &= ~1;
    if (p->triggerFlags & 0x40) {
        u8 *forced = &p->forced;
        if (*forced) {
            Anim_RestartSequence(&p->animation, manager->animationData, 0, (u8)p->sequence, 4);
            *forced = 0;
            switch (p->sequence) {
                case 0:
                case 1:
                    Sound_PlaySong(0x1AC);
                    break;
            }
        }
        if (!StepPlayback(self, &p->animation, p))
            goto tick;
        self->flags |= 1;
        p->state = 3;
        p->timer = 0;
        p->triggerFlags &= ~0x42;
        return;
    } else {
        TileTrigger_ApplyContactEffects(p);
    }
tick:
    p->timer++;
}
