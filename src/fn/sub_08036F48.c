#include "global.h"
struct Blob {
    u32 words[8];
};
struct LinkMenu {
    u8 task[0x18];
    u8 state, entered, pad1a[2];
    u32 timer;
    int window;
    void *background;
    void *script;
    void *resource;
    struct Blob blob;
    u8 draw50[8];
    u32 flags;
    u8 pad5c[0x54];
};
extern struct LinkMenu *gUnk_020000F8;
extern u16 gUnk_03004BD8, gUnk_03005260[];
extern u32 gUnk_0300523C;
extern u16 gUnk_03004FC0[];
void sub_08033C1C(void);
void PaletteLevels_Reset(void);
void sub_08215490(int);
void sub_08218B30(void);
void sub_08215114(void);
void sub_0821969C(void);
void sub_0821447C(int, void *, void *, void *);
void sub_08177DEC(int);
void Bg_ClearTilemap(int);
void *ScriptProc_Start(int, int);
void sub_082161B4(int, int, void *, int, int, int, void *);
void BgGfx_LoadTilemapSimple(int, int, int, int, int);
void sub_08036C0C(void);
void sub_0822B058(int);
void sub_08036E14(struct LinkMenu *);
void sub_08036EC8(struct LinkMenu *, int);
int sub_08033690(int, int, int, int);
void TextWindow_SetScript(int, void *);
void TextWindow_Refresh(int, int);
void sub_080337FC(int);
void sub_08033778(int);
void Sound_PlaySong(int);
int sub_08036BEC(void);
int sub_0824923C(void *, void *);
extern void *gUnk_08605770[];
void Anim_Remove(void *);
void sub_08036F48(struct LinkMenu *p)
{
    {
        int bits = 4;
        u32 *flags;
        flags = &gUnk_0300523C;
        *flags |= bits;
    }
    if (p->entered) {
        p->entered = 0;
        sub_08036C0C();
    }
    if (p->timer == 4) {
        sub_0822B058(1);
        sub_08036E14(p);
        sub_08036EC8(p, 2);
        p->flags &= ~1;
        p->window = sub_08033690(1, 14, 28, 6);
        if (p->window >= 0) {
            TextWindow_SetScript(p->window, p->script);
            TextWindow_Refresh(p->window, 0);
            sub_080337FC(p->window);
        }
        Sound_PlaySong(402);
    }
    p->timer++;
}