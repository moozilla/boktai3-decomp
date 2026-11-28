#include "monsters/garmr_statue.h"
#include "script/script_utility.h"
#include "system/game_state/game_state.h"
void Bestiary_UnlockEntry(s32);
void Player_AddExperience(s32);
#define H(p, n) (*(s16 *)((u8 *)(p) + (n)))
static inline s32 clamp(s32 v)
{
    if (v > 9999)
        v = 9999;
    else if (v < 0)
        v = 0;
    return v;
}
void GarmrStatue_HandleDefeat(struct GarmrStatue *p)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u32 script = p->unknown160;
    if (script) {
        values[0] = p->sprite.display.position.x;
        values[1] = p->sprite.display.position.y;
        values[2] = p->sprite.display.position.z;
        values[3] = p->unknown154;
        values[4] = p->unknown158;
        values[5] = p->unknown15C;
        values[6] = p->reactionFlags;
        values[7] = (s32)p->hud;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    Bestiary_UnlockEntry(30);
    if (p->reactionFlags & 1) {
        u16 flags;
        Player_AddExperience(p->unknown140);
        H(gGameState, 0x87C) = H(gGameState, 0x87C) < 9999 ? H(gGameState, 0x87C) + 1 : 9999;
        H(gGameState, 0x538) = H(gGameState, 0x538) < 9999 ? H(gGameState, 0x538) + 1 : 9999;
        flags = p->bodyReactionFlags;
        H(gGameState, 0x568) = H(gGameState, 0x568) < 9999 ? H(gGameState, 0x568) + 1 : 9999;
        if (flags & 0x80) {
            gGameState->targetFlagBonuses[0] = clamp(gGameState->targetFlagBonuses[0] + 1);
        } else if (flags & 0x100) {
            gGameState->targetFlagBonuses[1] = clamp(gGameState->targetFlagBonuses[1] + 1);
        } else if (flags & 0x200) {
            gGameState->targetFlagBonuses[2] = clamp(gGameState->targetFlagBonuses[2] + 1);
        } else if (flags & 0x400) {
            gGameState->targetFlagBonuses[3] = clamp(gGameState->targetFlagBonuses[3] + 1);
        } else if (flags & 0x800) {
            gGameState->targetFlagBonuses[4] = clamp(gGameState->targetFlagBonuses[4] + 1);
        }
    }
}
