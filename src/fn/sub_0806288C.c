#include "global.h"

void SceneInputPanel_ResetHighlights(u8 *, u8 *);
void Sound_PlaySong(s32);
s32 SceneInputPanel_AdjustSlidingY(u8 *, s32, u16);
void sub_08061C04(u8 *);

s32 sub_0806288C(u8 *p)
{
    u16 *q;
    SceneInputPanel_ResetHighlights(p, p + 0x1D00);
    q = (u16 *)(p + 0x1AEE);
    if (*q == 0)
        Sound_PlaySong(0x10E);
    if (SceneInputPanel_AdjustSlidingY(p + 0xCA8, 0x68, *q))
        sub_08061C04(p);
    return 0;
}
