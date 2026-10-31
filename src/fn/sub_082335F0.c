#include "global.h"
#include "engine.h"
#include "contact_position.h"
#include "ram.h"

struct ChargedBurstSpark {
    u32 flags;
    u8 unk_04[0x18 - 4];
    struct ContactPosition position;
    u8 unk_20[8];
    u8 active, timer;
    u8 unk_2A[2];
};
struct ChargedBurst {
    u8 unk_00[0x8C];
    struct ContactPosition position;
    u8 unk_94[0xA4 - 0x94];
    struct ChargedBurstSpark sparks[8];
    u8 nextSpark, sparkDelay;
};

#ifdef NONMATCHING
// fuzzy: 67.1%
// scope shape 34 (differences once low registers are set aside)
// Emits one spark every second call, cycling through the eight slots.
void ChargedBurst_EmitSpark(struct ChargedBurst *p)
{
    u8 *delayAddress = &p->sparkDelay;
    u32 delay = *delayAddress + 1;
    u32 byteMask;
    *delayAddress = delay;
    byteMask = 255;
    if ((u8)delay > 1) {
        const u16 *table;
        u32 *seed;
        u16 index;
        u32 mask;

        p->sparks[p->nextSpark].flags &= ~1;
        p->sparks[p->nextSpark].position = p->position;
        table = gRandomTable;
        seed = &gRandomIndex;
        index = *seed + 1;
        mask = 1023;
        index &= mask;
        {
            const u16 *sample = &table[index];
            u32 coordinate = (u16)p->sparks[p->nextSpark].position.x;
            coordinate -= 127;
            p->sparks[p->nextSpark].position.x = coordinate + (u8)*sample;
        }
        index = (index + 1) & mask;
        {
            const u16 *sample = &table[index];
            u32 coordinate = (u16)p->sparks[p->nextSpark].position.y;
            coordinate -= 127;
            p->sparks[p->nextSpark].position.y = coordinate + (u8)*sample;
        }
        index = (index + 1) & mask;
        *seed = index;
        {
            const u16 *sample = &table[index];
            u32 coordinate = (u16)p->sparks[p->nextSpark].position.z;
            coordinate -= 127;
            p->sparks[p->nextSpark].position.z = coordinate + (u8)*sample;
        }
        p->sparks[p->nextSpark].active = 1;
        p->sparks[p->nextSpark].timer = 0;
        {
            u32 next = p->nextSpark + 1;
            p->nextSpark = next;
            if ((next & byteMask) > 7)
                p->nextSpark = 0;
        }
        *delayAddress = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", ChargedBurst_EmitSpark);
#endif
