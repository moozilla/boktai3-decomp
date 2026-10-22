#include "global.h"
struct MenuDigit {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
    s16 x, y;
    u8 unknown24[0x3C];
};
struct MenuDisplay {
    u8 unknown0[0x1420];
    struct MenuDigit digits[1];
};
void SceneHud_FillDigitCells(s32, s32, s32, s32);
void MenuDisplay_LoadDecimalDigits(struct MenuDisplay *, s32, s32, s32, s32);

void InventoryPanel_SetEntryRow(struct MenuDisplay *p, s32 x, s32 y, s32 value, s32 frameBase, s32 first)
{
    s32 i;
    if (value < 0) {
        for (i = 0; i < 3; i++)
            p->digits[first + i].flags |= 1;
        SceneHud_FillDigitCells(3, x, y, 0);
    } else {
        SceneHud_FillDigitCells(3, x, y, -1);
        for (i = 0; i < 3; i++) {
            p->digits[first + i].x = x * 8 + i * 8;
            p->digits[first + i].y = y * 8;
            p->digits[first + i].flags &= ~1;
        }
        MenuDisplay_LoadDecimalDigits(p, value, 100, frameBase, first);
    }
}
