#define SceneHud_DrawActiveStatValue sub_08165518
#define SceneHud_DrawDecimalDigits   sub_08165488
#define SceneHud_FillDigitCells      sub_081654DC
#define gGameState                   gUnk_02000710
#define gPlayerSlots                 gUnk_02000580
#include "global.h"
#include "player.h"
#include "game_state.h"
s32 sub_0814A7D8(struct Player *, s32);
s32 Player_GetAttackPower(struct Player *);
void sub_08165488(s32, s32, s32, s32, s32);
void sub_081654DC(s32, s32, s32, s32);
void sub_08165518(s32 x1, s32 y1, s32 x2, s32 y2, s32 style, s32 mode)
{
    if ((s16)gGameState->unk50 <= 7) {
        sub_08165488(sub_0814A7D8(gPlayerSlots[0], mode), 100, x1, y1, style);
        sub_081654DC(3, x2, y2, style);
    } else {
        sub_081654DC(3, x1, y1, style);
        sub_08165488(Player_GetAttackPower(gPlayerSlots[0]), 100, x2, y2, style);
    }
}
