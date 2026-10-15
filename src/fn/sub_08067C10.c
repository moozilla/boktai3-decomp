// MATCHING_CONCESSION: register-pinning
#include "scene_particle_burst.h"
#define SceneParticleBurst_UpdateSlot sub_08067C10
s32 Div(s32, s32);
static inline u32 Step(struct DisplaySequenceState *state, struct SceneParticleEffect *sprite,
                       const u16 *graphicsInput)
{
    const u16 *graphics = graphicsInput;
    const volatile u16 *command =
        (const volatile u16 *)((u32)state->commands + (state->index << 1));
    u32 ended, duration, flags, bit;
    {
        u32 frame = *command >> 6;
        sprite->tile = ((sprite->width >> 3) * (sprite->height >> 3)) * frame + graphics[2];
    }
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
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
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
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
            state->duration = ((*command & 15) * state->speed) >> 6;
        } else {
            ended = 0;
        }
    }
    return ended;
}
void SceneParticleBurst_UpdateSlot(struct SceneParticleBurstPool *input,
                                   struct SceneParticleBurstSlot *inputSlot)
{
    struct SceneParticleBurstSlot *slot;
    struct SceneParticleBurstSlot *incoming = inputSlot;
    register struct SceneParticleBurstPool *p asm("r4");
    s32 initialIndex;
    register u32 effectOffset asm("r1");
    struct SceneParticleEffect *sprite;
    asm("" : "=r"(p) : "0"(input), "r"(incoming) : "r5");
    slot = incoming;
    initialIndex = slot->index;
    effectOffset = initialIndex * 0x28 + 0x20;
    sprite = (void *)((u8 *)p + effectOffset);
    if (slot->delay > 0) {
        slot->delay--;
        return;
    }
    sprite->flags &= ~1;
    slot->position.x += slot->velocity.x;
    slot->position.y += slot->velocity.y;
    slot->position.z += slot->velocity.z;
    slot->velocity.x = Div(slot->velocity.x * 7, 10);
    slot->velocity.y = Div(slot->velocity.y * 7, 10);
    slot->velocity.z = Div(slot->velocity.z * 7, 10);
    sprite->position = slot->position;
    {
        s32 index = slot->index;
        const u16 *graphics = p->graphicsResource;
        if (Step((void *)(index * 16 + (u32)p + 0x520), sprite, graphics))
            slot->state = 2;
    }
    slot->frame++;
}
