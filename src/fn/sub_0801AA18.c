#include "global.h"
// MATCHING_CONCESSION: register-pinning
extern u8 *gUnk_02000710;
void SceneHud_DrawClock(int);
void SceneHud_DrawNumericStatus(int);
void Text_DrawNumber(int, int, int, int, int, int, int);
void SceneInputPanel_GetEquippedValues(int *, int *, int, int);
void SceneHud_FillTile020(int, int, int);
int Player_GetCurrentAttribute(int);
void SelectionMenu_StepCursors(void *);
void SelectionMenu_DrawStatistics(u8 *p)
{
    int value, a, b;
    SceneHud_DrawClock(p[0x24]);
    SceneHud_DrawNumericStatus(p[0x24]);
    value = *(s16 *)(gUnk_02000710 + 0x40);
    if (value > 99)
        value = 99;
    else if (value < 0)
        value = 0;
    {
        int x = 10;
        asm("" : "+r"(x));
        Text_DrawNumber(0, x, 9, value, 33, 2, 15);
    }
    a = 0;
    b = 0;
    SceneInputPanel_GetEquippedValues(&a, &b, 0, 0);
    if (b > 0)
        Text_DrawNumber(0, 9, 10, b, 33, 3, 15);
    else
        SceneHud_FillTile020(9, 10, 3);
    if (a > 0)
        Text_DrawNumber(0, 9, 11, a, 33, 3, 15);
    else
        SceneHud_FillTile020(9, 11, 3);
    Text_DrawNumber(0, 11, 12, Player_GetCurrentAttribute(0), 33, 3, 15);
    Text_DrawNumber(0, 11, 13, Player_GetCurrentAttribute(1), 33, 3, 15);
    Text_DrawNumber(0, 11, 14, Player_GetCurrentAttribute(2), 33, 3, 15);
    SelectionMenu_StepCursors(p);
}
