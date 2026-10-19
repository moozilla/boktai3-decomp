#include "global.h"
#include "game_state.h"
#ifdef NONMATCHING
// fuzzy: 95.7%
// foundry draft (worker best 94.8%); what differs is mostly register allocation and instruction order
#define MenuSelection_DrawNumericSummary sub_08175494
u16 *Hud_GetTilemapCell(u32, u32, u32);
s32 Hud_GetMinimum(s32, s32);
void SceneHud_DrawDecimalDigits(s32, s32, s32, s32, s32);
void sub_081655AC(s32, s32, s32);
s32 SceneCatalog_GetLevelScaledValue(u8 *);
void sub_08175494(u8 *s)
{
    if ((*(u8 **)(s + 0xA40))[0x418] != 6) {
        s32 zero;
        s32 max;
        s32 divisor;
        s32 value;
        SceneHud_DrawDecimalDigits(Hud_GetMinimum(gUnk_02000710->level, 99), 10, 21, 3, 0);
        zero = 0;
        value = Hud_GetMinimum(gUnk_02000710->rewardTotal, max = 999999);
        SceneHud_DrawDecimalDigits(value, divisor = 100000, 23, 4, zero);
        if (gUnk_02000710->level > 98) {
            u16 *p = Hud_GetTilemapCell(0, 23, 5);
            s32 i;
            u32 tile = 0xF020;
            i = 5;
            do {
                *p++ = tile;
            } while (--i >= 0);
        } else {
            SceneHud_DrawDecimalDigits(Hud_GetMinimum(SceneCatalog_GetLevelScaledValue(s), max),
                                       divisor, 23, 5, zero);
        }
    } else {
        SceneHud_DrawDecimalDigits(Hud_GetMinimum(gUnk_02000710->level + 10, 99), 10, 21, 3, 0);
        sub_081655AC(6, 23, 4);
        sub_081655AC(6, 23, 5);
        sub_081655AC(2, 20, 8);
        SceneHud_DrawDecimalDigits(99, 10, 25, 8, 0);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08175494);
#endif
