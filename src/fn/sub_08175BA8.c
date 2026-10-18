#include "global.h"
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
void Hud_SetConfigFirstValueBounds(u8 *);
void Hud_SetConfigSecondValueBounds(u8 *);
void Hud_SetConfigThirdValueBounds(u8 *);
void MenuSelection_RefreshAllOptionText(u8 *s)
{
    {
        s32 *id = (s32 *)(s + 0x48D0);
        TextWindow_SetScript(*id, *(const u8 **)(s + 0x5C));
        TextWindow_Refresh(*id, s[0x48C1] + 28);
        Hud_SetConfigFirstValueBounds(s);
        TextWindow_StartPlayback(*id);
    }
    {
        s32 *id = (s32 *)(s + 0x48D8);
        TextWindow_SetScript(*id, *(const u8 **)(s + 0x5C));
        TextWindow_Refresh(*id, s[0x48C2] + 31);
        Hud_SetConfigSecondValueBounds(s);
        TextWindow_StartPlayback(*id);
    }
    {
        s32 *id = (s32 *)(s + 0x48E0);
        TextWindow_SetScript(*id, *(const u8 **)(s + 0x5C));
        TextWindow_Refresh(*id, 35 - s[0x48C3]);
        Hud_SetConfigThirdValueBounds(s);
        TextWindow_StartPlayback(*id);
    }
}
