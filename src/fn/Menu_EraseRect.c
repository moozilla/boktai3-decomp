#include "global.h"
void TextWindow_BlankRect(void);
void sub_08218ED8(s32, s32, s32, s32);
void Menu_EraseRect(s32 a, s32 b, s32 c, s32 d)
{
    TextWindow_BlankRect();
    sub_08218ED8(a, b, c, d);
}
