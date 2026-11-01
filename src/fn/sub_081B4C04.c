#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
#include "display_sequence_inline.h"

struct VanargandPart {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x1c];
    struct DisplaySequenceState sequence;
    u8 motion[0x38];
    struct SceneVec *basePosition;
    u8 unknown94[8];
    struct SceneVec origin;
    struct SceneVec target;
    s16 heading, distance, particleAngle;
    u8 unknownB2[0xaa];
    u32 variant, flags, state, timer;
    u8 currentAction, previousAction, reset, enter, finished;
    u8 unknown171[7];
    u32 actionTimer;
    u8 unknown17C[0x20];
    u32 flashTimer;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
extern const s16 gUnk_086149C4[];
static inline void StepSequence(struct VanargandPart *owner)
{
    struct DisplaySequenceState *state;
    const u16 *command;
    state = &owner->sequence;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = owner->sequence.commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->sprite.frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
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

// confidence=low: holds sequence 11 for 180 frames before restoring action 22.
static inline void ChangeAction(struct VanargandPart *p, u8 action)
{
    u32 zero = 0;
    if ((u8)(p->previousAction - 25) <= 1 && !p->finished)
        return;
    p->previousAction = action;
    p->currentAction = action;
    p->reset = 1;
    p->finished = zero;
    p->actionTimer = zero;
    p->state = zero;
    p->timer = zero;
}
void Vanargand_UpdatePartRecovery(u8 *boss, struct VanargandPart *p, s32 frame)
{
    u32 mask = 12;
    p->flags |= mask;
    p->flags |= 0x120;
    if (p->reset) {
        p->reset = 0;
        {
            void *resource = *(void **)(boss + 0x4d0);
            Anim_SetSequence(&p->sequence, resource, 11, (u8)p->variant, 0);
        }
        StepSequence(p);
    }
    if (frame > 180) {
        {
            u32 offset = 0x160;
            u32 *flags;
            u32 mask;
            flags = (u32 *)((u8 *)p + offset);
            mask = ~0x100;
            *flags &= mask;
        }
        ChangeAction(p, 22);
    }
}
