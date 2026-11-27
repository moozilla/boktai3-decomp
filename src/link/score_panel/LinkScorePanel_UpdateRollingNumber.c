#include "global.h"
#include "link/link_score_panel.h"
s32 Mod(s32, s32);
void Sound_PlaySong(s32);
void LinkMenu_DrawRandomizedFourDigitNumber(struct LinkScorePanel *, s32);
void LinkScorePanel_UpdateRollingNumber(struct LinkScorePanel *s)
{
    if (Mod(s->h, 2) == 0)
        Sound_PlaySong(0x2A5);
    if (s->h2 != 0)
        LinkMenu_DrawRandomizedFourDigitNumber(s, 0);
}
