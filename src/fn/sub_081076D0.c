// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"
#define gTileTriggerManager gUnk_020001B8
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
extern struct TileTriggerManager *gUnk_020001B8;
void sub_08106E98(struct TileTrigger *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
extern u32 gUnk_030053F4;
void TileTrigger_EmitParticles(struct TileTrigger *);
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
// confidence=low: plays the hold sequence while the 0x20 trigger flag is set; moves to state 6 when it ends.
void TileTrigger_StepHold(struct TileTrigger *p)
{
    struct TileTrigger *self;
    struct TileTriggerManager *manager;
    manager = gTileTriggerManager;
    self = p;
    if (p->triggerFlags & 0x20) {
        p->flags &= ~1;
        {
            u8 *forced = &p->forced;
            if (*forced) {
                TileTrigger_EmitParticles(p);
                Anim_SetSequence(&p->animation, manager->animationData, 0, (u8)p->sequence, 0);
                *forced = 0;
                switch (p->sequence) {
                    case 0:
                    case 1:
                        if (gUnk_030053F4 & 0x800)
                            Sound_PlaySong(0x26D);
                        else
                            Sound_PlaySong(0x1AB);
                        break;
                }
            }
        }
        if ((p->animation.index == 0 && p->animation.elapsed > 2) ||
            (p->animation.index == 1 && p->animation.elapsed != 0))
            sub_08106E98(p);
        if (StepPlayback(self, &p->animation, p)) {
            p->state = 6;
            p->triggerFlags = (p->triggerFlags & ~0x20) | 2;
        }
    }
}
