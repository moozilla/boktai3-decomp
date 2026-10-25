#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"
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
};
void sub_08106E98(struct TileTrigger *);
static inline u32 StepPlayback(struct TileTrigger *owner, struct DisplaySequenceState *state,
                               struct TileTrigger *base)
{
    struct TileTrigger *sprite = owner;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, flags, bit;
    u32 duration;
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
        u32 maskBit = 1;
        bit = maskBit;
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
        u32 first;
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        u32 temp;
        u32 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
        mask = temp;
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
                s32 weight = state->frameWeight;
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
#ifdef NONMATCHING
// fuzzy: 84.7%
// fix.py best (sweep after the inlined-insn fix)
// Remaining: the savedDuration copy is emitted before the PRE-inserted
// &p->frame and lands in r5, swapping with mask (ROM: saved r7, mask r5).
inline static u32 StepPlayback_StepPress(struct TileTrigger *owner,
                                         struct DisplaySequenceState *state,
                                         struct TileTrigger *base)
{
    struct TileTrigger *sprite = owner;
    const volatile u16 *command;
    u32 ended;
    u32 flags;
    u32 bit;
    u32 duration;
    typeof(state->speed) t1;
    typeof(flags) c1;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->animation.commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->spriteFrame = (*command) >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        u8 first;
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        u32 temp;
        u32 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
        mask = temp;
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            c1 = flags;
            if (c1 & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= c1;
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
                    endFlag = c1 & 8;
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
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            state->frameWeight = (*command) & 15;
            {
                s32 weight = state->frameWeight;
                s32 speed;
                t1 = state->speed;
                speed = t1;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!((u8)speed))
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

void TileTrigger_StepPress(struct TileTrigger *p)
{
    struct TileTrigger *self = p;
    p->flags &= ~1;
    if ((p->animation.index == 1) && (p->animation.elapsed != 0)) {
        sub_08106E98(p);
        p->frame++;
    } else {
        StepPlayback_StepPress(self, &p->animation, p);
    }
    if (p->frame > 30) {
        p->state = 5;
        p->forced = 1;
        p->frame = 0;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", TileTrigger_StepPress);
#endif
