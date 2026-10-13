#include "global.h"
void TextWindow_Refresh(u32, u32);
void Sound_PlaySong(u32);
void sub_080379A4(u8 *p)
{
    if (p[0x1a] != 0) {
        p[0x1a] = 0;
        TextWindow_Refresh(*(u32 *)(p + 0x4EC), 0);
        Sound_PlaySong(0x192);
    }
    *(u32 *)(p + 0x20) += 1;
}
