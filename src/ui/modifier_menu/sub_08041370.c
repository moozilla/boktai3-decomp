#include "global.h"

extern u8 *gGameState;
void TextWindow_SetScript(s32, u32);
void TextWindow_SetString(s32, u32, u8 *);
void TextWindow_Refresh(s32, u32);
void TextWindow_StartPlayback(s32);
void ModifierMenu_DrawSelectedLabels(u8 *);
void ModifierMenu_DrawAvailableLabels(u8 *);

void ModifierMenu_RefreshTextLabels(u8 *p)
{
    s32 *q = (s32 *)(p + 0xf0);
    if (*q >= 0) {
        TextWindow_SetScript(*q, *(u32 *)(p + 0xbc));
        TextWindow_SetString(*q, 0, gGameState + 0x790);
        TextWindow_Refresh(*q, 3);
        TextWindow_StartPlayback(*q);
    }
    switch (*(s32 *)(p + 0x18)) {
        case 0:
            ModifierMenu_DrawSelectedLabels(p);
            break;
        case 1:
            ModifierMenu_DrawAvailableLabels(p);
            break;
        case 2:
            ModifierMenu_DrawSelectedLabels(p);
            break;
    }
}
