// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine_globals.h"
#include "ram.h"
// confidence=low: moves one frost particle, steps playback and hides it on expiry.
#define FrostEmitter_UpdateParticle sub_081D2EA4
struct FrostVec {
    u16 x, y, z, pad;
};
struct FrostSequence {
    const u16 *commands;
    u8 flags, count, weight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct FrostParticle {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[10];
    struct FrostVec position;
    u8 unknown24[8];
    struct FrostSequence sequence;
    struct FrostVec velocity;
    u16 active, unknown46;
};
struct FrostEmitter {
    u8 unknown0[0x18];
    u8 graphics[0x1C];
    void *resource;
    struct FrostVec *source;
    u32 emitting;
    struct FrostParticle particles[12];
    u32 enabled, age;
};
extern const s16 gUnk_086149C4[];
void Anim_SetSequence(void *, void *, u16, u8, u8);
void FrostEmitter_UpdateParticle(struct FrostParticle *input)
{
    register struct FrostParticle *p asm("ip") = input;
    {
        register struct FrostVec *position asm("r1") = &p->position;
        register struct FrostVec *velocity asm("r2") = &p->velocity;
        {
            register struct FrostParticle *base asm("r3") = p;
            register u32 x asm("r0");
            x = base->velocity.x;
            asm("" : "+r"(x));
            base->position.x = x + base->position.x;
        }
        {
            register u32 value asm("r0") = velocity->y;
            asm("" : "+r"(value));
            position->y = value + position->y;
        }
        {
            register u32 value asm("r0") = velocity->z;
            register u32 current asm("r4");
            current = position->z;
            position->z = value + current;
        }
    }
    {
        struct FrostSequence *state = &p->sequence;
        struct FrostParticle *base;
        const volatile u16 *command; /* preserve observed packed-command reads */
        u32 ended, duration, flags, bit;
        {
            u32 offset = state->index << 1;
            const volatile u16 *table;
            base = p;
            table = base->sequence.commands;
            command = (const volatile u16 *)((u32)table + offset);
        }
        {
            u32 frame = *command >> 6;
            base->frame = frame;
        }
        {
            u32 input = state->flags;
            register u32 first asm("r1");
            u32 maskBit = 1;
            bit = maskBit;
            asm("" : "+r"(bit));
            first = bit;
            first &= input;
            if (first != (((*command & 0x30) >> 4) & bit)) {
                register struct FrostParticle *dst asm("r3") = p;
                dst->flags |= 4;
            } else {
                u32 value;
                {
                    register struct FrostParticle *src asm("r1") = p;
                    value = src->flags;
                }
                {
                    u32 mask = ~4;
                    value &= mask;
                }
                {
                    register struct FrostParticle *dst asm("r2") = p;
                    dst->flags = value;
                }
            }
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
            if (first != (((*command & 0x30) >> 4) & bit)) {
                register struct FrostParticle *dst asm("r3") = p;
                dst->flags |= 8;
            } else {
                u32 value;
                {
                    register struct FrostParticle *src asm("r1") = p;
                    value = src->flags;
                }
                {
                    u32 mask = ~8;
                    value &= mask;
                }
                {
                    register struct FrostParticle *dst asm("r2") = p;
                    dst->flags = value;
                }
            }
        }
        {
            u32 elapsed = state->elapsed + 1;
            u32 mask;
            register u32 temp asm("r3");
            register u32 savedDuration asm("r3");
            state->elapsed = elapsed;
            temp = 0xFFFF;
            asm("" : "+r"(temp));
            mask = temp;
            /* Keep the live mask distinct from its source without binding r7. */
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
                            state->index = state->count - 1;
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
                    frameCount = state->count;
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
                state->weight = *command & 15;
                {
                    register s32 weight asm("r1") = state->weight;
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
    }
    {
        u16 *active = &p->active;
        u32 remaining;
        remaining = *active - 1;
        *active = remaining;
        if (!(u16)remaining) {
            register struct FrostParticle *dst asm("r4") = p;
            dst->flags |= 1;
        }
    }
}
