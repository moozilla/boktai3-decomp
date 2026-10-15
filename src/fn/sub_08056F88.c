// MATCHING_CONCESSION: register-pinning
#include "level_advance_display.h"
void LevelAdvanceDisplay_StartParticle(struct LevelAdvanceParticle *particle)
{
    const u16 *table;
    register u32 *randomIndex asm("r12");
    register u32 mask asm("r8");
    register u32 index asm("r2");
    s32 x, y, z;
    u32 rangeMask;
    u32 zero;
    particle->flags &= ~1;
    table = (const u16 *)RANDOM_TABLE_ADDR;
    randomIndex = &gUnk_03005308;
    index = *randomIndex + 1;
    {
        u32 firstMask = 1023;
        mask = firstMask;
        index &= firstMask;
    }
    {
        s32 value = table[index];
        value >>= 3;
        rangeMask = 127;
        x = (value & rangeMask) - 63;
    }
    zero = 0;
    asm("" : "+r"(zero));
    particle->offsetX = x;
    index = (index + 1) & mask;
    {
        s32 value = table[index];
        y = (value >> 3 & 63) + 64;
    }
    particle->offsetY = y;
    index = (index + 1) & mask;
    *randomIndex = index;
    {
        s32 value = table[index];
        z = (value >> 3 & rangeMask) - 63;
    }
    particle->offsetZ = z;
    particle->position.world.x = gUnk_02000580[0]->position.world.x + x;
    particle->position.world.y = gUnk_02000580[0]->position.world.y + y;
    particle->position.world.z = gUnk_02000580[0]->position.world.z + z;
    {
        u32 value = zero;
        particle->velocityX = value;
    }
    index = (index + 1) & mask;
    *randomIndex = index;
    {
        register u32 value asm("r1") = table[index];
        register u32 lowMask asm("r2") = 7;
        particle->velocityY = (value & lowMask) + 12;
    }
    {
        register u32 value asm("r1") = zero;
        particle->velocityZ = value;
        particle->frame = value;
    }
    particle->active = 1;
}
