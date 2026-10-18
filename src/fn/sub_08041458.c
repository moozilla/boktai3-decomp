#include "global.h"

void TextWindow_Destroy(s32);

void ModifierMenu_DestroyTextWindows(u8 *p)
{
    s32 m = -1;
    s32 *e = (s32 *)(p + 0xf0);
    s32 i = 7;
    do {
        if (*e >= 0) {
            TextWindow_Destroy(*e);
            *e = m;
        }
        e++;
        i--;
    } while (i >= 0);
}
