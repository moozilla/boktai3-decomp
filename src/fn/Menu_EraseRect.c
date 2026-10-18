#include "global.h"
void TextWindow_BlankRect(void);
void BackgroundMap_ClearTextRect(s32, s32, s32, s32);
void Menu_EraseRect(s32 a, s32 b, s32 c, s32 d)
{
    TextWindow_BlankRect();
    BackgroundMap_ClearTextRect(a, b, c, d);
}
