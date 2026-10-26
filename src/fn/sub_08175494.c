#include "global.h"
#include "game_state.h"
u16 *Hud_GetTilemapCell(u32, u32, u32);
s32 Hud_GetMinimum(s32, s32);
void SceneHud_DrawDecimalDigits(s32, s32, s32, s32, s32);
void ResultPanel_FillTileRun(s32, s32, s32);
s32 SceneCatalog_GetLevelScaledValue(u8 *);
void MenuSelection_DrawNumericSummary(u8 *s)
{
    if ((*(u8 **)(s + 0xA40))[0x418] != 6) {
        s32 zero;
        s32 max;
        s32 divisor;
        s32 value;
        SceneHud_DrawDecimalDigits(Hud_GetMinimum(gGameState->level, 99), 10, 21, 3, 0);
        zero = 0;
        value = Hud_GetMinimum(gGameState->rewardTotal, max = 999999);
        SceneHud_DrawDecimalDigits(value, divisor = 100000, 23, 4, zero);
        if (gGameState->level > 98) {
            u16 *p = Hud_GetTilemapCell(0, 23, 5);
            s32 i;
            i = 0;
            do {
                *p++ = 0xF020;
            } while (++i < 6);
        } else {
            SceneHud_DrawDecimalDigits(Hud_GetMinimum(SceneCatalog_GetLevelScaledValue(s), max),
                                       divisor, 23, 5, zero);
        }
    } else {
        SceneHud_DrawDecimalDigits(Hud_GetMinimum(gGameState->level + 10, 99), 10, 21, 3, 0);
        ResultPanel_FillTileRun(6, 23, 4);
        ResultPanel_FillTileRun(6, 23, 5);
        ResultPanel_FillTileRun(2, 20, 8);
        SceneHud_DrawDecimalDigits(99, 10, 25, 8, 0);
    }
}
