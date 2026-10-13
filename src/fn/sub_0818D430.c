#include "global.h"

void Sound_PlaySongChecked(u32);
void sub_0818D460(void);

void sub_0818D430(u8 *p)
{
    u32 c = *(u16 *)(p + 0x1D6) - 1;
    *(u16 *)(p + 0x1D6) = c;
    if ((c << 16) == 0) {
        *(u32 *)(p + 0x1D0) = (u32)sub_0818D460;
        Sound_PlaySongChecked(0x16A);
    }
}
