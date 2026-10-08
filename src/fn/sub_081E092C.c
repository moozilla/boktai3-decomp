#include "global.h"
extern u32 gUnk_02000388;
void Menu_EraseRect(s32, s32, s32, s32);
u32 Script_ParseStringRef(u32);
u32 Text_LookupString(u32);
void Menu_DrawText(s32, s32, u32);
void sub_081E092C(void)
{
    Menu_EraseRect(4, 0xd, 0x16, 2);
    Menu_DrawText(4, 0xd, Text_LookupString(Script_ParseStringRef(gUnk_02000388) + 0xa));
}
