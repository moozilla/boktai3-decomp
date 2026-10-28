// MATCHING_CONCESSION: register-pinning
#include "burst_emitter.h"
void sub_08126AD4(const struct ContactPosition *, s32, s32, s32, s32, s32, u32);
void SpriteParticle_CreateVariant4384(const struct ContactPosition *, s32, s32, s32, s32, s32, s32,
                                      s32, s32, s32, s32, s32, s32);
void BurstEmitter_EmitFrame(struct BurstEmitter *emitter)
{
    s32 index;
    switch (emitter->frame) {
        case 0:
            if (!emitter->context)
                Sound_PlaySong(0x165);
            for (index = 0; index <= 5; index++)
                sub_08126AD4(&emitter->position, 1, 0, index << 6, 84, 1, emitter->context);
            break;
        case 1: {
            struct ContactPosition *position;
            u32 one;
            index = 0;
            position = &emitter->position;
            one = 1;
            for (; index <= 3; index++) {
                sub_08126AD4(position, 1, 8, index << 6, 34, one, emitter->context);
                sub_08126AD4(position, 1, 14, index << 6, 64, one, emitter->context);
            }
            break;
        }
        case 2: {
            struct ContactPosition *position;
            const u16 *table;
            u32 *current;
            index = 0;
            position = &emitter->position;
            asm("" : "+r"(position) : "r"(index) : "r0", "r1", "r2", "r3");
            table = gRandomTable;
            current = &gRandomIndex;
            for (; index <= 5; index++) {
                u32 random = (*current + 1) & 0x3FF;
                s32 value;
                *current = random;
                value = *(u16 *)((random << 1) + (u32)table);
                value &= 15;
                sub_08126AD4(position, 1, value, index << 6, 150, 1, emitter->context);
            }
            break;
        }
        case 3: {
            struct ContactPosition *position;
            const u16 *table;
            u32 *current;
            index = 0;
            position = &emitter->position;
            asm("" : "+r"(position) : "r"(index) : "r0", "r1", "r2", "r3");
            table = gRandomTable;
            current = &gRandomIndex;
            for (; index <= 7; index++) {
                u32 random = (*current + 1) & 0x3FF;
                s32 value;
                *current = random;
                value = *(u16 *)((random << 1) + (u32)table);
                value &= 31;
                sub_08126AD4(position, 1, value, index << 5, 100, 1, emitter->context);
            }
            break;
        }
        case 4: {
            register struct ContactPosition *position asm("r4");
            u32 *current;
            u32 mask;
            const u16 *table;
            index = 0;
            position = &emitter->position;
            current = &gRandomIndex;
            mask = 0x3FF;
            table = gRandomTable;
            for (; index <= 15; index++) {
                register u32 random asm("r1") = *current;
                s32 value;
                s32 duration;
                random = (random + 1) & mask;
                value = *(u16 *)((random << 1) + (u32)table);
                value &= 15;
                value += 5;
                random = (random + 1) & mask;
                *current = random;
                duration = *(u16 *)((random << 1) + (u32)table);
                duration &= 63;
                duration += 80;
                sub_08126AD4(position, 1, value, index << 4, duration, 2, emitter->context);
            }
            break;
        }
        case 5:
            if (!emitter->context) {
                u32 kind = 2;
                for (index = 0; index <= 7; index++)
                    SpriteParticle_CreateVariant4384(&emitter->position, 8, 16, 4, 12, 18,
                                                     index << 5, 8, 64, 1, kind, 512, 8);
            }
            break;
    }
}
