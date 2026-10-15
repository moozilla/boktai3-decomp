// MATCHING_CONCESSION: register-pinning
#include "spark_particle.h"
#define SparkParticle_UpdatePool sub_08069FA4
s32 SparkParticle_UpdatePool(struct SparkParticlePool *pool)
{
    if (pool->activeCount) {
        u32 one = 1;
        u32 two = 2;
        u8 *active;
        register struct SparkParticle *particle asm("r4");
        u32 eight;
        s32 remaining;
        active = &pool->particles[0].active;
        particle = pool->particles;
        eight = 8;
        remaining = 47;
        asm("" : "+r"(active), "+r"(particle), "+r"(eight), "+r"(remaining));
        do {
            if (*active) {
                register struct DisplaySequenceState *state asm("r4") = &particle->sequence;
                const volatile u16 *command;
                const struct SparkParticleResource *resource;
                s32 finished;
                u32 elapsed, duration, flags;
                u32 mask;
                register u32 cache asm("r2");
                u32 zero;
                particle->sprite.position.x += particle->velocity.x;
                particle->sprite.position.y += particle->velocity.y;
                particle->sprite.position.z += particle->velocity.z;
                resource = pool->graphics;
                {
                    u32 offset = state->index << 1;
                    register const u16 *table asm("r1") = state->commands;
                    command = (const volatile u16 *)((u32)table + offset);
                }
                {
                    u32 frame = *command >> 6;
                    u32 width = particle->sprite.width >> 3;
                    u32 height = particle->sprite.height >> 3;
                    height *= width;
                    height *= frame;
                    height += resource->tileBase;
                    particle->sprite.tile = height;
                }
                {
                    u32 input = state->flags;
                    register u32 first asm("r2") = one;
                    first &= input;
                    if (first != (((*command & 0x30) >> 4) & one))
                        particle->sprite.flags |= 4;
                    else
                        particle->sprite.flags &= ~4;
                }
                asm("" : "+r"(state));
                {
                    u32 input = state->flags;
                    register u32 first asm("r1") = two;
                    first &= input;
                    first = (u8)first;
                    if (first != (((*command & 0x30) >> 4) & two))
                        particle->sprite.flags |= eight;
                    else
                        particle->sprite.flags &= ~8;
                }
                elapsed = state->elapsed + 1;
                zero = 0;
                state->elapsed = elapsed;
                {
                    register u32 temp asm("r2") = 0xFFFF;
                    asm("" : "+r"(temp));
                    mask = temp;
                }
                duration = state->duration;
                elapsed = (u16)elapsed;
                cache = duration;
                if (elapsed >= duration) {
                    state->elapsed = zero;
                    flags = state->flags;
                    if (flags & 4) {
                        u32 index = state->index;
                        if (!index) {
                            u32 hold = eight;
                            hold &= flags;
                            asm("" : "+r"(hold));
                            if (hold) {
                                state->elapsed = cache;
                            } else {
                                state->index = state->frameCount - 1;
                            }
                            finished = 1;
                        } else {
                            state->index = index - 1;
                            finished = 0;
                        }
                    } else {
                        u32 index = state->index + 1;
                        state->index = index;
                        cache = state->frameCount;
                        if ((index & mask) >= cache) {
                            u32 hold = flags & eight;
                            asm("" : "+r"(hold));
                            if ((u8)hold) {
                                state->elapsed = duration;
                                {
                                    u32 next = cache - 1;
                                    asm("" : "+r"(next) : "r"(cache));
                                    state->index = next;
                                }
                            } else
                                state->index = 0;
                            finished = 1;
                        } else
                            finished = 0;
                    }
                    {
                        u32 offset = state->index << 1;
                        register const u16 *table asm("r1") = state->commands;
                        command = (const volatile u16 *)((u32)table + offset);
                    }
                    state->duration = ((*command & 15) * (s32)state->speed) >> 6;
                } else
                    finished = 0;
                if (finished) {
                    *active = 0;
                    {
                        u32 flags = particle->sprite.flags;
                        u32 mask = one;
                        asm("" : "+r"(flags), "+r"(mask));
                        particle->sprite.flags = flags | mask;
                    }
                    {
                        register u32 offset asm("r2") =
                            (u32) & ((struct SparkParticlePool *)0)->activeCount;
                        u32 *count;
                        count = (u32 *)((u8 *)pool + offset);
                        (*count)--;
                    }
                }
            }
            active += sizeof(*particle);
            particle++;
            {
                s32 step = -1;
                register s32 test asm("r1");
                remaining += step;
                test = remaining;
                if (test < 0)
                    break;
            }
        } while (1);
    }
    return 0;
}
