#include "global.h"
struct S {
    u8 f[0x1026];
    u16 h;
    u8 p[6];
    u16 h2;
};
s32 Mod(s32, s32);
void Sound_PlaySong(s32);
void LinkMenu_DrawRandomizedFourDigitNumber(struct S *, s32);
void LinkScorePanel_UpdateRollingNumber(struct S *s)
{
    if (Mod(s->h, 2) == 0)
        Sound_PlaySong(0x2A5);
    if (s->h2 != 0)
        LinkMenu_DrawRandomizedFourDigitNumber(s, 0);
}
