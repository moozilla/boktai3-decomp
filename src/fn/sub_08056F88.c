#include "level_advance_display.h"
void LevelAdvanceDisplay_StartParticle(struct LevelAdvanceParticle *particle)
{
    const u16 *table;
    u32 *randomIndex;
    u32 index;
    s32 x, y, z;
    u32 rangeMask;
    particle->sprite.flags &= ~1;
    table = gRandomTable;
    randomIndex = &gRandomIndex;
    index = *randomIndex + 1;
    index &= 1023;
    {
        s32 value = table[index];
        value >>= 3;
        rangeMask = 127;
        x = (value & rangeMask) - 63;
    }
    particle->offsetX = x;
    index = (index + 1) & 1023;
    {
        s32 value = table[index];
        y = (value >> 3 & 63) + 64;
    }
    particle->offsetY = y;
    index = (index + 1) & 1023;
    *randomIndex = index;
    {
        s32 value = table[index];
        z = (value >> 3 & rangeMask) - 63;
    }
    particle->offsetZ = z;
    particle->sprite.position.x = gPlayers[0]->position.x + x;
    particle->sprite.position.y = gPlayers[0]->position.y + y;
    particle->sprite.position.z = gPlayers[0]->position.z + z;
    particle->velocityX = 0;
    index = ((*(&index)) + 1) & 1023;
    *randomIndex = index;
    {
        u32 value = table[index];
        particle->velocityY = (value & 7) + 12;
    }
    particle->velocityZ = 0;
    particle->frame = 0;
    particle->active = 1;
}
