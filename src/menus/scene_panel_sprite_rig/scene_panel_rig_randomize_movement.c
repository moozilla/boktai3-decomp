#include "engine/shared/scene_panel_rig.h"
#include "player/player.h"
#include "engine/shared/monster_particle_random.h"
static inline s32 randomGlobal(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)gRandomTable);
}
void ScenePanelRig_RandomizeMovement(struct ScenePanelRig *input)
{
    struct ScenePanelRig *rig = input;
    s32 angle, difference;
    const u16 *table;
    u32 *seed;
    u32 mask;
    s32 facing;
    ScenePanelRig_InitMovement(rig);
    {
        union ParticleRandomTableSource source;
        source.halves[0] = (u16)RANDOM_TABLE_ADDR;
        source.halves[1] = RANDOM_TABLE_ADDR >> 16;
        table = ParticleRandom_GetTable(&source);
    }
    seed = &gRandomIndex;
    {
        u32 index = *seed + 1;
        u32 sample;
        mask = 0x3FF;
        index &= mask;
        *seed = index;
        PARTICLE_RANDOM_READ_SAMPLE(sample, table, *seed);
        rig->speed = (u8)sample + 192;
    }
    angle = Math_Atan2(gPlayers[0]->position.x - rig->origin.x,
                       gPlayers[0]->position.z - rig->origin.z);
    facing = rig->angle >> 4;
    angle -= 256;
    difference = (facing - angle) & 255;
    if (difference <= 127)
        rig->turnRate = (ParticleRandom_NextSample(table, seed, mask) & 7) + 12;
    else
        rig->turnRate = -((ParticleRandom_NextSample(table, seed, mask) & 7) + 12);
    rig->turnStep = 0;
    rig->turnDuration = (randomGlobal() & 63) + 64;
    rig->motionFlag = 0;
}
