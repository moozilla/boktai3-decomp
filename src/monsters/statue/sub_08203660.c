#include "script/script_utility.h"
#include "monsters/statue.h"
#include "global.h"
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
void Statue_RecordDefeat(struct Statue *p)
{
    s32 values[8];
    struct ScriptUtilityArguments m;
    u32 q = p->unknown14C;
    if (q) {
        values[0] = p->sprite.position.x;
        values[1] = p->sprite.position.y;
        values[2] = p->sprite.position.z;
        values[3] = p->unknown168;
        values[4] = p->unknown16C;
        values[5] = p->unknown170;
        values[6] = p->hitFlags;
        values[7] = (s32)p->owner;
        m.count = 8;
        m.values = values;
        Script_Call(q, &m);
    }
    Bestiary_UnlockEntry(28);
    if (p->hitFlags & 1) {
        u16 flags;
        Player_AddExperience(p->unknown158);
        H(gGameState, 0x87C) = H(gGameState, 0x87C) < 9999 ? H(gGameState, 0x87C) + 1 : 9999;
        H(gGameState, 0x538) = H(gGameState, 0x538) < 9999 ? H(gGameState, 0x538) + 1 : 9999;
        flags = p->bodyReactionFlags;
        H(gGameState, 0x574) = H(gGameState, 0x574) < 9999 ? H(gGameState, 0x574) + 1 : 9999;
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
