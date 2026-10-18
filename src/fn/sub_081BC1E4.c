#include "global.h"
#define SpriteMenu_InitText sub_081BC1E4
struct SpriteMenuText { u8 pad0[0x6F0]; const void *text; };
s32 Script_SeekToKeyword(u32);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
void MessageWindow_SetPalette(int);
const void *Script_GetTextSourcePc(void);
void sub_081BC1E4(struct SpriteMenuText *menu)
{
    if (Script_SeekToKeyword('r'))
        menu->text = Script_GetTextSourcePc();
    MessageWindow_SetPalette(0x2E9);
    MessageWindow_SetMessageTable(menu->text);
    MessageWindow_SetBounds(1, 15, 28, 4);
    MessageWindow_SetDisplayMode(1);
}
