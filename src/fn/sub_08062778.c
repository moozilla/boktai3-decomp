#include "global.h"

void SceneInputPanel_ResetHighlights(u8 *, u8 *);
void Sound_PlaySong(s32);
s32 SceneInputPanel_AdjustSlidingY(u8 *, s32, u16);
void sub_08061B48(u8 *);

s32 sub_08062778(u8 *p)
{
    u16 *q;
    SceneInputPanel_ResetHighlights(p, p + 0x1D00);
    q = (u16 *)(p + 0x1AEE);
    if (*q == 0)
        Sound_PlaySong(0x276);
    if (SceneInputPanel_AdjustSlidingY(p + 0xB88, 0x20, *q))
        sub_08061B48(p);
    return 0;
}
