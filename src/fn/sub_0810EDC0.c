#include "global.h"
#include "area_effect_link.h"
struct H {
    u8 filler[0x14E4];
    u32 a;
    u8 pad[8];
    u32 c;
    u32 d;
};
struct G2 {
    u8 f[0x53];
    u8 b;
    u8 p[0xcc];
    u8 a[0x10];
    u8 c[0x30];
    u8 e[0x10];
    u8 g[1];
};
void TextWindow_SetScript(u32, u32);
void TextWindow_Refresh(u32, u32);
void TextWindow_SetString(u32, u32, void *);
void TextWindow_StartPlayback(u32);
void LinkLobbyPanel_RefreshPlayerNames(struct H *h)
{
    struct G2 *g = gLinkSession;
    if (g != 0 && g->b == 1 && g->a != 0) {
        TextWindow_SetScript(h->c, h->a);
        TextWindow_Refresh(h->c, 0);
        TextWindow_SetString(h->c, 1, g->c);
        TextWindow_StartPlayback(h->c);
        g = gLinkSession;
        if (g != 0 && g->b == 1 && g->e != 0) {
            TextWindow_SetScript(h->d, h->a);
            TextWindow_Refresh(h->d, 1);
            TextWindow_SetString(h->d, 2, g->g);
            TextWindow_StartPlayback(h->d);
        }
    }
}
