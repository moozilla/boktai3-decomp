// MATCHING_CONCESSION: register-pinning
#include "scene_panel_rig.h"
#include "player.h"
static inline s32 randomValue(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}
static inline u32 randomByte(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u8 *)(offset + (u32)table);
}
static inline s32 randomGlobal(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)gUnk_0203B400);
}
void ScenePanelRig_RandomizeMovement(struct ScenePanelRig *input)
{
    struct ScenePanelRig *rig = input;
    s32 angle, difference;
    register const u16 *table asm("r5");
    u32 zero;
    s32 facing;
    ScenePanelRig_InitMovement(rig);
    table = gUnk_0203B400;
    rig->speed = randomByte(table) + 192;
    angle = Math_Atan2(gPlayers[0]->position.x - rig->origin.x,
                       gPlayers[0]->position.z - rig->origin.z);
    facing = rig->angle >> 4;
    angle -= 256;
    difference = (facing - angle) & 255;
    if (difference <= 127)
        rig->turnRate = (randomValue(table) & 7) + 12;
    else
        rig->turnRate = -((randomValue(table) & 7) + 12);
    {
        u16 *counter = (u16 *)&rig->turnStep;
        zero = 0;
        asm("" : "+r"(zero));
        *counter = 0;
    }
    rig->turnDuration = (randomGlobal() & 63) + 64;
    rig->motionFlag = zero;
}
