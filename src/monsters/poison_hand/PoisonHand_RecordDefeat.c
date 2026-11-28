#include "global.h"
#include "engine/engine.h"
#include "monsters/poison_hand.h"
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

void PoisonHand_RecordDefeat(struct PoisonHand *hand)
{
    s32 values[8];
    struct ScriptUtilityArguments args;
    u32 script = hand->unknown164;

    if (script) {
        values[0] = hand->sprite.position.x;
        values[1] = hand->sprite.position.y;
        values[2] = hand->sprite.position.z;
        values[3] = hand->unknown158;
        values[4] = hand->unknown15C;
        values[5] = hand->unknown160;
        values[6] = hand->reaction;
        values[7] = (s32)hand->hud;
        args.count = 8;
        args.values = values;
        Script_Call(script, &args);
    }
    Bestiary_UnlockEntry(32);
    if (hand->reaction & 1) {
        u16 flags;
        Player_AddExperience(hand->unknown148);
        IncrementCounter(&gGameState->counter87C);
        IncrementCounter(&gGameState->counter538);
        flags = hand->bodyReactionFlags;
        IncrementCounter(&gGameState->counter56C);
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
