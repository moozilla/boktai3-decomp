#include "script/script_utility.h"
#include "monsters/screamer/screamer_state.h"
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
void Screamer_HandleDefeat(struct ScreamerState *p)
{
    s32 values[8];
    struct ScriptUtilityArguments m;
    u32 q = p->f214;
    if (q) {
        values[0] = p->sprite.position.x;
        values[1] = p->sprite.position.y;
        values[2] = p->sprite.position.z;
        values[3] = p->f208;
        values[4] = p->f20C;
        values[5] = p->f210;
        values[6] = p->f200;
        values[7] = (s32)p->f1F8;
        m.count = 8;
        m.values = values;
        Script_Call(q, &m);
    }
    Bestiary_UnlockEntry(33);
    if (p->f200 & 1) {
        u16 flags;
        Player_AddExperience(p->f1F0);
        H(gGameState, 0x87C) = H(gGameState, 0x87C) < 9999 ? H(gGameState, 0x87C) + 1 : 9999;
        H(gGameState, 0x538) = H(gGameState, 0x538) < 9999 ? H(gGameState, 0x538) + 1 : 9999;
        flags = p->f1E8;
        H(gGameState, 0x56A) = H(gGameState, 0x56A) < 9999 ? H(gGameState, 0x56A) + 1 : 9999;
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
