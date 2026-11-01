#ifndef GUARD_SCENE_ACTOR_CONTROL_STEP_H
#define GUARD_SCENE_ACTOR_CONTROL_STEP_H
#include "scene_actor_control.h"
#include "display_sprite_playback.h"
/* PlayerCue_AdvanceCommand for a control slot. The frame and the first flip
 * bit go through slot, the second flip bit through sprite, a second pointer
 * to the same slot (the creators keep it in its own register). */
// confidence=low: why the original used two pointers is unknown.
static inline void SceneActorControl_StepSprite(struct SceneActorControlSlot *slot,
                                                struct SceneActorControlSlot *sprite,
                                                struct DisplaySequenceState *state)
{
    const u16 *command;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    slot->tile = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            slot->spriteFlags |= 4;
        else
            slot->spriteFlags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->spriteFlags |= 8;
        else
            sprite->spriteFlags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else
                state->index--;
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else
                    state->index = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    }
}
#endif
