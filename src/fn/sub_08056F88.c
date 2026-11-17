#include "level_advance_display.h"
void LevelAdvanceDisplay_StartParticle(struct LevelAdvanceParticle *particle)
{
    const u16 *table;
    u32 *randomIndex;
    u32 mask;
    u32 index;
    s32 x, y, z;
    u32 rangeMask;
    particle->sprite.flags &= ~1;
    table = (const u16 *)RANDOM_TABLE_ADDR;
    randomIndex = &gRandomIndex;
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
    particle->sprite.position.x = gPlayers[0]->position.x + x;
    particle->sprite.position.y = gPlayers[0]->position.y + y;
    particle->sprite.position.z = gPlayers[0]->position.z + z;
    {
        u32 value = 0;
        particle->velocityX = value;
    }
    index = ((*(&index)) + 1) & mask;
    *randomIndex = index;
    {
        u32 value = table[index];
        u32 lowMask = 7;
        particle->velocityY = (value & lowMask) + 12;
    }
    {
        u32 value = 0;
        particle->velocityZ = value;
        particle->frame = value;
    }
    particle->active = 1;
}
