#define TextLabel_DisplaySuffix sub_08174440
#define TextLabel_FormatSuffix sub_081743F4
#include "global.h"
/* Tentative semantic names; field offsets are established by static accesses. */
struct TextLabelStyle {
    u8 index;
    u8 unknown1[0xB];
    u8 specialC;
    u8 unknownD[3];
    u8 special10;
    u8 unknown11[3];
    u8 suffixKind;
};
void sub_081743F4(struct TextLabelStyle *, u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_SetString(int, int, void *);
s32 MessageWindow_ShowMessage(s32);
int MessageWindow_SetStringArgument(int, void *);
void sub_08174440(s32 useWindow, s32 id, struct TextLabelStyle *style, u8 *text)
{
    sub_081743F4(style, text);
    if (useWindow) {
        TextWindow_Refresh(id, style->index);
        TextWindow_SetString(id, 0, text);
    } else {
        MessageWindow_ShowMessage(style->index);
        MessageWindow_SetStringArgument(0, text);
    }
}
