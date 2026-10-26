// MATCHING_CONCESSION: register-pinning
#include "scene_actor_placement.h"
struct SceneActorEmissionPhase {
    u8 unknown0[0x2B6];
    u8 direction, unknown2B7[3];
    u8 amplitude;
    s8 amplitudeStep;
    u16 timer;
};
s32 sub_080148CC(s32, s32, const struct ContactPosition *, u32, s32, s32, s32, s32, s32, s32, s32);
void SceneActorPlacement_EmitTimedParticles(struct SceneActorPlacementView *actor)
{
    struct SceneActorEmissionPhase *phase = (void *)actor;
    u32 angle;
    struct ContactPosition offset;
    u32 category;
    if (!phase->timer)
        return;
    --phase->timer;
    angle = (((phase->direction + 9) & 7) << 5);
    angle += phase->amplitude;
    phase->amplitude += phase->amplitudeStep * 4;
    if (phase->amplitude > 64) {
        s32 step;
        phase->amplitude = 64;
        step = phase->amplitudeStep;
        phase->amplitudeStep = -step;
    }
    if (Mod(phase->timer, 4))
        return;
    Memory_Clear(&offset, 8);
    category = actor->category;
    switch (category) {
        case 2:
            offset.y = 0;
            if (!actor->paletteOffset) {
                union {
                    struct ContactPosition position;
                    u32 words[2];
                } position;
                const u16 *table = gUnk_0203B400;
                u32 mask;
                {
                    register u32 index asm("r2");
                    {
                        u32 *indexPointer = &gRandomIndex;
                        index = *indexPointer + 1;
                        mask = 1023;
                        index &= mask;
                    }
                    {
                        u32 lowMask = 65535;
                        u32 copiedWord;
                        s32 oldY;
                        u32 highY;
                        index = (index + 1) & mask;
                        /* Copy first: the following word update reads only initialized X/Y. */
                        position.position = actor->position;
                        copiedWord = position.words[0];
                        oldY = (s32)copiedWord >> 16;
                        highY = (u16)offset.y + oldY;
                        highY <<= 16;
                        lowMask &= copiedWord;
                        lowMask |= highY;
                        position.words[0] = lowMask;
                    }
                    {
                        s32 count;
                        s32 duration;
                        const u16 *sample;
                        index = (index + 1) & mask;
                        count = table[index];
                        count &= 3;
                        count += 2;
                        index = (index + 1) & mask;
                        gRandomIndex = index;
                        sample = (const u16 *)((index * sizeof(*table)) + (u32)table);
                        {
                            u32 value = *sample;
                            value &= 3;
                            value += 6;
                            duration = value;
                        }
                        sub_080148CC(count, 9, &position.position, angle, 8, duration, 4, 10, 8, 5,
                                     10);
                    }
                }
                {
                    register u32 index asm("r0");
                    u32 direction;
                    register u32 angleMask asm("r8");
                    s32 duration;
                    index = (gRandomIndex + 1) & mask;
                    direction = table[index];
                    direction &= 7;
                    direction += angle;
                    angleMask = 255;
                    direction &= angleMask;
                    index = (index + 1) & mask;
                    gRandomIndex = index;
                    {
                        u32 value = table[index];
                        value &= 7;
                        value += 6;
                        duration = value;
                    }
                    sub_080148CC(3, 9, &position.position, direction, 8, duration, 4, 14, 4, 6, 10);
                    asm("" : : : "r9");
                    {
                        register u32 index asm("r1");
                        u32 direction;
                        s32 count;
                        s32 duration;
                        index = (gRandomIndex + 1) & mask;
                        direction = table[index];
                        direction &= 15;
                        direction += angle;
                        direction &= angleMask;
                        index = (index + 1) & mask;
                        count = table[index];
                        count &= 3;
                        count += 1;
                        index = (index + 1) & mask;
                        gRandomIndex = index;
                        {
                            u32 value = table[index];
                            value &= 7;
                            value += 4;
                            duration = value;
                        }
                        sub_080148CC(count, 9, &position.position, direction, 8, duration, 2, 6, 3,
                                     5, 10);
                    }
                }
            } else {
                union {
                    struct ContactPosition position;
                    u32 words[2];
                } position;
                const u16 *table = gUnk_0203B400;
                u32 mask;
                {
                    register u32 index asm("r2");
                    {
                        u32 *indexPointer = &gRandomIndex;
                        index = *indexPointer + 1;
                        mask = 1023;
                        index &= mask;
                    }
                    {
                        u32 lowMask = 65535;
                        u32 copiedWord;
                        s32 oldY;
                        u32 highY;
                        index = (index + 1) & mask;
                        /* Copy first: the following word update reads only initialized X/Y. */
                        position.position = actor->position;
                        copiedWord = position.words[0];
                        oldY = (s32)copiedWord >> 16;
                        highY = (u16)offset.y + oldY;
                        highY <<= 16;
                        lowMask &= copiedWord;
                        lowMask |= highY;
                        position.words[0] = lowMask;
                    }
                    {
                        s32 count;
                        s32 duration;
                        const u16 *sample;
                        index = (index + 1) & mask;
                        count = table[index];
                        count &= 3;
                        count += 2;
                        index = (index + 1) & mask;
                        gRandomIndex = index;
                        sample = (const u16 *)((index * sizeof(*table)) + (u32)table);
                        {
                            u32 value = *sample;
                            value &= 3;
                            value += 6;
                            duration = value;
                        }
                        sub_080148CC(count, 10, &position.position, angle, 8, duration, 4, 10, 8, 5,
                                     10);
                    }
                }
                {
                    register u32 index asm("r0");
                    u32 direction;
                    u32 angleMask;
                    s32 duration;
                    index = (gRandomIndex + 1) & mask;
                    direction = table[index];
                    direction &= 7;
                    direction += angle;
                    angleMask = 255;
                    direction &= angleMask;
                    index = (index + 1) & mask;
                    gRandomIndex = index;
                    {
                        u32 value = table[index];
                        value &= 7;
                        value += 6;
                        duration = value;
                    }
                    sub_080148CC(3, 10, &position.position, direction, 8, duration, 4, 14, 4, 6,
                                 10);
                    asm("" : : : "r8", "r9");
                    {
                        register u32 index asm("r1");
                        u32 direction;
                        s32 count;
                        s32 duration;
                        index = (gRandomIndex + 1) & mask;
                        direction = table[index];
                        direction &= 15;
                        direction += angle;
                        direction &= angleMask;
                        index = (index + 1) & mask;
                        count = table[index];
                        count &= 3;
                        count += 1;
                        index = (index + 1) & mask;
                        gRandomIndex = index;
                        {
                            u32 value = table[index];
                            value &= 7;
                            value += 4;
                            duration = value;
                        }
                        sub_080148CC(count, 10, &position.position, direction, 8, duration, 2, 6, 3,
                                     5, 10);
                    }
                }
            }
            break;
        case 14:
            offset.y = 40;
            {
                union {
                    struct ContactPosition position;
                    u32 words[2];
                } position;
                register const u16 *table asm("r6") = gUnk_0203B400;
                u32 mask;
                {
                    register u32 index asm("r2");
                    {
                        u32 *indexPointer = &gRandomIndex;
                        index = *indexPointer + 1;
                        mask = 1023;
                        index &= mask;
                    }
                    {
                        u32 lowMask = 65535;
                        u32 copiedWord;
                        s32 oldY;
                        u32 highY;
                        index = (index + 1) & mask;
                        /* Copy first: the following word update reads only initialized X/Y. */
                        position.position = actor->position;
                        copiedWord = position.words[0];
                        oldY = (s32)copiedWord >> 16;
                        highY = (u16)offset.y + oldY;
                        highY <<= 16;
                        lowMask &= copiedWord;
                        lowMask |= highY;
                        position.words[0] = lowMask;
                    }
                    {
                        s32 count;
                        s32 duration;
                        const u16 *sample;
                        index = (index + 1) & mask;
                        count = table[index];
                        count &= 3;
                        count += 2;
                        index = (index + 1) & mask;
                        gRandomIndex = index;
                        sample = (const u16 *)((index * sizeof(*table)) + (u32)table);
                        {
                            u32 value = *sample;
                            value &= 3;
                            value += 6;
                            duration = value;
                        }
                        sub_080148CC(count, 11, &position.position, angle, 8, duration, 4, 10, 8, 5,
                                     10);
                    }
                }
                {
                    register u32 index asm("r0");
                    u32 direction;
                    register u32 angleMask asm("r9");
                    s32 duration;
                    index = (gRandomIndex + 1) & mask;
                    direction = table[index];
                    direction &= 7;
                    direction += angle;
                    angleMask = 255;
                    direction &= angleMask;
                    index = (index + 1) & mask;
                    gRandomIndex = index;
                    {
                        u32 value = table[index];
                        value &= 7;
                        value += 6;
                        duration = value;
                    }
                    sub_080148CC(3, 11, &position.position, direction, 8, duration, 4, 14, 4, 6,
                                 10);
                    asm("" : : : "r8");
                    {
                        register u32 index asm("r1");
                        u32 direction;
                        s32 count;
                        s32 duration;
                        index = (gRandomIndex + 1) & mask;
                        direction = table[index];
                        direction &= 15;
                        direction += angle;
                        direction &= angleMask;
                        index = (index + 1) & mask;
                        count = table[index];
                        count &= 3;
                        count += 1;
                        index = (index + 1) & mask;
                        gRandomIndex = index;
                        {
                            u32 value = table[index];
                            value &= 7;
                            value += 4;
                            duration = value;
                        }
                        sub_080148CC(count, 11, &position.position, direction, 8, duration, 2, 6, 3,
                                     5, 10);
                    }
                }
            }
            break;
    }
}
