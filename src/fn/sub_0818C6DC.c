#include "global.h"

void Sound_PlaySongChecked(u32);

void sub_0818C6DC(u32 a, u32 b, u8 *p)
{
    if (*(p + 0x11F) == 6) {
        Sound_PlaySongChecked(0x1E4);
    }
}
