#include "global.h"
#include "ui/town/solar_bank_panel.h"
s32 Script_SeekToKeyword(u32);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
void MessageWindow_SetPalette(int);
const void *Script_GetTextSourcePc(void);
void SpriteMenu_InitText(struct SolarBankPanel *panel)
{
    if (Script_SeekToKeyword('r'))
        panel->text = Script_GetTextSourcePc();
    MessageWindow_SetPalette(0x2E9);
    MessageWindow_SetMessageTable(panel->text);
    MessageWindow_SetBounds(1, 15, 28, 4);
    MessageWindow_SetDisplayMode(1);
}
