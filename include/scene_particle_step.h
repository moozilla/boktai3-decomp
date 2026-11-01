#ifndef GUARD_SCENE_PARTICLE_STEP_H
#define GUARD_SCENE_PARTICLE_STEP_H
#include "scene_particle_pool.h"
#include "display_sprite_playback.h"
/* PlayerCue_AdvanceCommand for a particle effect: the tile is the frame
 * times the effect's 8x8 cell count plus the graphics base, and the next
 * duration is not clamped to 1. Returns 1 when a cycle ends. */
static inline u32 SceneParticle_StepSprite(struct DisplaySequenceState *state,
                                           struct SceneParticleEffect *sprite, const u16 *graphics)
{
    const u16 *command;
    u32 ended;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    {
        u32 frame = *command >> 6;
        sprite->tile = ((sprite->width >> 3) * (sprite->height >> 3)) * frame + graphics[2];
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        state->duration = ((*command & 15) * state->speed) >> 6;
    } else {
        ended = 0;
    }
    return ended;
}
#endif
