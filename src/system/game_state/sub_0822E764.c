#include "global.h"
#include "system/game_state/game_state.h"
#include "engine/engine.h"
s32 GameState_GetUnlockEntry(s32 index);
void GameState_SetUnlockSlot(s32 index, s32 value);
s32 GameState_HasUnlockedInventoryEntry(s32 value);

void sub_0822E764(s32 a, s32 b)
{
    s32 entry = GameState_GetUnlockEntry(a);
    s32 i, n, sel;
    s32 list[8];
    s32 *p;

    GameState_SetUnlockSlot(a, GameState_GetUnlockEntry(b));
    GameState_SetUnlockSlot(b, entry);
    for (i = 0; i <= 7; i++) {
        if (*(i + &gGameState->inventory[8]) == a)
            *(i + &gGameState->inventory[8]) = b;
        else if (*(i + &gGameState->inventory[8]) == b)
            *(i + &gGameState->inventory[8]) = a;
    }
    n = 0;
    for (i = 0, p = list; i <= 11; i++) {
        if (GameState_HasUnlockedInventoryEntry(i)) {
            *p++ = i;
            n++;
        }
    }
    sel = -1;
    if ((s16)gGameState->unk50 > 7) {
        for (i = 0; i < n; i++) {
            if (*((s16)gGameState->unk50 + gGameState->inventory) == list[i]) {
                sel = i;
                break;
            }
        }
    }
    for (; n <= 7; n++)
        list[n] = -1;
    for (i = 0; i <= 7; i++)
        *(i + &gGameState->inventory[8]) = list[i];
    if ((u32)sel <= 7)
        *(s16 *)&gGameState->unk50 = sel + 8;
}
