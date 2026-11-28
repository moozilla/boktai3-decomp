#include "global.h"
#include "engine/engine.h"
#include "monsters/popbud/popbud_playback.h"
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

void Popbud_RecordDefeat(struct PinkSpineState *popbud)
{
    s32 values[8];
    struct ScriptUtilityArguments args;
    u32 script = popbud->f158;

    if (script) {
        values[0] = popbud->sprite.position.x;
        values[1] = popbud->sprite.position.y;
        values[2] = popbud->sprite.position.z;
        values[3] = popbud->f14C;
        values[4] = popbud->f150;
        values[5] = popbud->f154;
        values[6] = popbud->f144;
        values[7] = (s32)popbud->f140;
        args.count = 8;
        args.values = values;
        Script_Call(script, &args);
    }
    Bestiary_UnlockEntry(2);
    if (popbud->f144 & 1) {
        Player_AddExperience(popbud->f13C);
        IncrementCounter(&gGameState->counter87C);
        IncrementCounter(&gGameState->counter538);
        IncrementCounter((u16 *)gGameState->unk56E);
        gGameState->targetFlagBonuses[0] = clamp(gGameState->targetFlagBonuses[0] + 1);
    }
}
