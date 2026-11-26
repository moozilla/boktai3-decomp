#include "engine/shared/spark_particle.h"
#include "engine/shared/display_sequence_inline.h"
s32 SparkParticle_UpdatePool(struct SparkParticlePool *pool)
{
    if (pool->activeCount) {
        s32 i;
        for (i = 0; i < 48; i++) {
            struct SparkParticle *particle = &pool->particles[i];
            struct DisplaySequenceState *state = &particle->sequence;
            if (particle->active) {
                const u16 *command;
                u32 ended;
                particle->sprite.position.x += particle->velocity.x;
                particle->sprite.position.y += particle->velocity.y;
                particle->sprite.position.z += particle->velocity.z;
                {
                    const struct SparkParticleResource *graphics = pool->graphics;
                    {
                        u32 offset = DisplaySequence_GetIndex(state) << 1;
                        const u16 *table;
                        table = state->commands;
                        command = (const u16 *)((u32)table + offset);
                    }
                    {
                        u32 frame = *command >> 6;
                        particle->sprite.tile =
                            ((particle->sprite.width >> 3) * (particle->sprite.height >> 3)) *
                                frame +
                            graphics->tileBase;
                    }
                }
                {
                    union DisplaySequenceControl control;
                    u32 bit, first;
                    control.raw = state->flags;
                    bit = 1;
                    first = (u8)(control.raw & bit);
                    if (first != (((*command & 0x30) >> 4) & bit))
                        particle->sprite.flags |= 4;
                    else
                        particle->sprite.flags &= ~4;
                }
                {
                    union DisplaySequenceControl control;
                    u32 bit, first;
                    control.raw = state->flags;
                    bit = 2;
                    first = (u8)(control.raw & bit);
                    if (first != (((*command & 0x30) >> 4) & bit))
                        particle->sprite.flags |= 8;
                    else
                        particle->sprite.flags &= ~8;
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
                if (ended) {
                    particle->active = 0;
                    particle->sprite.flags |= 1;
                    pool->activeCount--;
                }
            }
        }
    }
    return 0;
}
