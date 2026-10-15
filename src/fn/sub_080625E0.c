#include "global.h"

void sub_0805F2CC(u8 *, u8 *);
s32 SceneInput_TickChoice(u8 *, u8 *, u8 *, u8 *);
void Sound_PlaySong(s32);
void sub_08061A40(u8 *);
void sub_08061B48(u8 *);

s32 sub_080625E0(u8 *p)
{
    s32 r;
    sub_0805F2CC(p, p + 0x1D00);
    r = SceneInput_TickChoice(p + 0x1C8, p + 0x20, p + 0x1AAA, p + 0x1AAF);
    if (r == 1) {
        sub_08061A40(p);
    } else if (r == 0) {
        Sound_PlaySong(0xde);
        sub_08061B48(p);
    }
    return 0;
}
