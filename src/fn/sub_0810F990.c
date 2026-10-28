#include "global.h"
#include "area_effect_link.h"
struct G {
    u8 f[0x53];
    u8 b;
};
struct S {
    u8 f[0xFF0];
    u32 e;
    u8 p[8];
    s32 i;
    u32 pad;
    u32 h;
};
void TextWindow_SetScript(u32, u32);
void TextWindow_Refresh(u32, u32);
void TextWindow_SetString(u32, u32, void *);
void TextWindow_StartPlayback(u32);
static inline u8 ok(s32 v)
{
    if (v < 0)
        goto z;
    if (v <= 1)
        goto o;
z:
    return 0;
o:
    return 1;
}
void LinkScorePanel_RefreshPlayerName(struct S *s)
{
    s32 i = s->i;
    struct G *g = gLinkSession;
    if (g != 0 && g->b == 1 && ok(i)) {
        u8 *q = (u8 *)g + ((i << 6) + 0x120);
        if (q != 0) {
            TextWindow_SetScript(s->h, s->e);
            TextWindow_Refresh(s->h, 0);
            TextWindow_SetString(s->h, 1, q + 0x10);
            TextWindow_StartPlayback(s->h);
        }
    }
}
