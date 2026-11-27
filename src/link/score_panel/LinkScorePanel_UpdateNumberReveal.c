#include "global.h"
struct S {
    u8 f[0x102E];
    u16 h2;
    u16 h3;
};
void Sound_PlaySong(s32);
void LinkScorePanel_SetNumber(struct S *, s32, s32);
void LinkScorePanel_UpdateNumberReveal(struct S *s)
{
    u16 *p = &s->h2;
    if (*p != 0) {
        *p -= 1;
        if (*p == 0) {
            LinkScorePanel_SetNumber(s, 0, s->h3);
            Sound_PlaySong(0x167);
        }
    }
}
