#include "effects/bursts/charged_burst.h"
#include "engine/math/monster_particle_random.h"
#include "engine/ram.h"

// Emits one spark every second call, cycling through the eight slots. Each spark
// starts at the burst's position, offset by -127 to +128 on each axis.
void ChargedBurst_EmitSpark(struct ChargedBurst *burst)
{
    u8 *delayAddress = &burst->sparkDelay;
    u32 delay = *delayAddress + 1;
    u32 byteMask;
    *delayAddress = delay;
    byteMask = 255;
    if ((u8)(delay & byteMask) > 1) {
        const u16 *table;
        union ParticleRandomTableSource source;
        u32 *seed;
        u16 index;
        u32 mask;

        burst->sparks[burst->nextSpark].sprite.flags &= ~1;
        burst->sparks[burst->nextSpark].sprite.position = burst->core.position;
        source.halves[0] = (u16)RANDOM_TABLE_ADDR;
        source.halves[1] = RANDOM_TABLE_ADDR >> 16;
        table = ParticleRandom_GetTable(&source);
        seed = &gRandomIndex;
        index = *seed + 1;
        mask = 1023;
        index &= mask;
        {
            const u16 *sample = ParticleRandom_SampleAddress(table, index);
            u32 coordinate = (u16)burst->sparks[burst->nextSpark].sprite.position.x;
            coordinate -= 127;
            burst->sparks[burst->nextSpark].sprite.position.x = coordinate + (u8)*sample;
        }
        index = (index + 1) & mask;
        {
            const u16 *sample = ParticleRandom_SampleAddress(table, index);
            u32 coordinate = (u16)burst->sparks[burst->nextSpark].sprite.position.y;
            coordinate -= 127;
            burst->sparks[burst->nextSpark].sprite.position.y = coordinate + (u8)*sample;
        }
        index = (index + 1) & mask;
        *seed = index;
        {
            const u16 *sample = ParticleRandom_SampleAddress(table, index);
            u32 coordinate = (u16)burst->sparks[burst->nextSpark].sprite.position.z;
            coordinate -= 127;
            burst->sparks[burst->nextSpark].sprite.position.z = coordinate + (u8)*sample;
        }
        burst->sparks[burst->nextSpark].active = 1;
        burst->sparks[burst->nextSpark].timer = 0;
        {
            u32 next = burst->nextSpark + 1;
            burst->nextSpark = next;
            if ((next & byteMask) > 7)
                burst->nextSpark = 0;
        }
        *delayAddress = 0;
    }
}
