#include "global.h"
#include "area_effect_link.h"
struct S {
    u32 f0[7];
    u32 g;
    u8 f[0xFE4 - 0x20];
    void *fp;
    u32 a;
    u32 b[2];
    u32 c;
    u32 d[2];
    u8 st;
    u8 pad[7];
    u32 h;
    u8 q[0x18];
    u8 c24;
    u8 pad2;
    u16 t;
};
struct G {
    u8 f[0x1c];
    u8 b;
};
void LinkScorePanel_HideSprites(struct S *);
void Bg_ClearTilemap(s32);
void Bg_SetLayerMap(s32, u32, s32);
void BgGfx_LoadTilemapSimple(s32, s32, s32, s32, s32);
void TextWindow_Clear(u32);
void TextWindow_SetBounds(u32, s32, s32, s32, s32);
void TextWindow_SetScript(u32, u32);
void TextWindow_Refresh(u32, u32);
void TextWindow_StartPlayback(u32);
void Registry1745_RequestHandshakeStart(void);
void SaveMenu_UpdateSaveDelay(void);
static inline void set(struct S *s, void *fp, u8 st)
{
    s->fp = fp;
    s->c24 = 1;
    s->st = st;
}
void LinkScorePanel_OnWaitSessionReady(struct S *s)
{
    struct G *g;
    s32 v;
    if (s->c24 != 0) {
        u8 z;
        LinkScorePanel_HideSprites(s);
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(3);
        Bg_SetLayerMap(0, s->g, 3);
        BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
        TextWindow_Clear(s->h);
        TextWindow_SetBounds(s->h, 1, 6, 0x1c, 6);
        TextWindow_SetScript(s->h, s->a);
        TextWindow_Refresh(s->h, 8);
        TextWindow_StartPlayback(s->h);
        Registry1745_RequestHandshakeStart();
        s->c24 = 0;
    }
    g = gLinkSession;
    v = -1;
    if (g != 0)
        v = g->b;
    if (v == 0x19)
        set(s, SaveMenu_UpdateSaveDelay, 4);
}
