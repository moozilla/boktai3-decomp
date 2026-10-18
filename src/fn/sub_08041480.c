#include "global.h"

void TextWindow_StopPlayback(s32);

void ModifierMenu_StopListTextPlayback(u8 *p)
{
    s32 *e = (s32 *)(p + 0xf8);
    s32 i = 5;
    do {
        if (*e >= 0) {
            TextWindow_StopPlayback(*e);
        }
        e++;
        i--;
    } while (i >= 0);
}
