#include "global.h"
#include "player.h"
#include "game_state.h"
s32 Player_CalculateTotalAttackPower(struct Player *, s32);
s32 Player_GetAttackPower(struct Player *);
void SceneHud_DrawDecimalDigits(s32, s32, s32, s32, s32);
void SceneHud_FillDigitCells(s32, s32, s32, s32);
void SceneHud_DrawActiveStatValue(s32 x1, s32 y1, s32 x2, s32 y2, s32 style, s32 mode)
{
    if ((s16)gGameState->unk50 <= 7) {
        SceneHud_DrawDecimalDigits(Player_CalculateTotalAttackPower(gPlayers[0], mode), 100, x1, y1,
                                   style);
        SceneHud_FillDigitCells(3, x2, y2, style);
    } else {
        SceneHud_FillDigitCells(3, x1, y1, style);
        SceneHud_DrawDecimalDigits(Player_GetAttackPower(gPlayers[0]), 100, x2, y2, style);
    }
}
