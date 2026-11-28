#include "global.h"
#include "engine/engine.h"
#include "monsters/muspell_head.h"
#include "script/script_utility.h"
#include "system/game_state/game_state.h"

void Bestiary_UnlockEntry(s32);
void Player_AddExperience(s32);

static inline s32 clamp(s32 v)
{
    if (v > 9999)
        v = 9999;
    else if (v < 0)
        v = 0;
    return v;
}
/* Capped at 9999; the comparison reads the counter as signed. */
static inline void IncrementCounter(u16 *value)
{
    *value = (s16)*value < 9999 ? *value + 1 : 9999;
}

void MuspellHead_RecordDefeat(struct MuspellHead *head)
{
    s32 values[8];
    struct ScriptUtilityArguments args;
    u32 script = head->optionD;

    if (script) {
        values[0] = head->sprite.position.x;
        values[1] = head->sprite.position.y;
        values[2] = head->sprite.position.z;
        values[3] = head->optionG0;
        values[4] = head->optionG1;
        values[5] = head->optionG2;
        values[6] = head->previousHit;
        values[7] = (s32)head->parent;
        args.count = 8;
        args.values = values;
        Script_Call(script, &args);
    }
    Bestiary_UnlockEntry(31);
    if (head->previousHit & 1) {
        u16 flags;
        Player_AddExperience(head->optionA);
        IncrementCounter(&gGameState->counter87C);
        IncrementCounter(&gGameState->counter538);
        flags = head->damageType;
        IncrementCounter(&gGameState->counter57C);
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
