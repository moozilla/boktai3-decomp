#include "script/script_utility.h"
#include "monsters/popbud/popbud_playback.h"
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
void Popbud_RecordDefeat(struct PinkSpineState *p)
{
    s32 values[8];
    struct ScriptUtilityArguments m;
    u32 q = p->f158;
    if (q) {
        values[0] = p->sprite.position.x;
        values[1] = p->sprite.position.y;
        values[2] = p->sprite.position.z;
        values[3] = p->f14C;
        values[4] = p->f150;
        values[5] = p->f154;
        values[6] = p->f144;
        values[7] = (s32)p->f140;
        m.count = 8;
        m.values = values;
        Script_Call(q, &m);
    }
    Bestiary_UnlockEntry(2);
    if (p->f144 & 1) {
        Player_AddExperience(p->f13C);
        H(gGameState, 0x87C) = H(gGameState, 0x87C) < 9999 ? H(gGameState, 0x87C) + 1 : 9999;
        H(gGameState, 0x538) = H(gGameState, 0x538) < 9999 ? H(gGameState, 0x538) + 1 : 9999;
        H(gGameState, 0x56E) = H(gGameState, 0x56E) < 9999 ? H(gGameState, 0x56E) + 1 : 9999;
        gGameState->targetFlagBonuses[0] = clamp(gGameState->targetFlagBonuses[0] + 1);
    }
}
