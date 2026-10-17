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
void Hud_SetMode(int);
void Bg_ClearTilemap(int);
void *ScriptProc_Start(int, int);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void BgGfx_LoadTilemapSimple(int, int, int, int, int);
void sub_08036C0C(void);
void sub_0822B058(int);
void sub_08036E14(void);
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
int Script_SeekToKeyword(int);
void *sub_08227E90(void);
void ResourceHeader_Relocate(void *, void *);
void sub_0821983C(void *, void *, int, int, int, int, int, int);
void *Script_GetKeywordValue(int, int);
int sub_08037140(struct LinkMenu *p)
{
    struct Blob *resource;
    struct Blob *blob;
    int zero;
    gUnk_020000F8 = p;
    PaletteLevels_Reset();
    if (!Script_SeekToKeyword(115))
        goto failed;
    p->script = sub_08227E90();
    if (!p->script)
        goto failed;
    resource = ScriptProc_Start(0xCB05, 0x7C8B);
    if (!resource)
        goto failed;
    p->blob = *resource;
    blob = &p->blob;
    ResourceHeader_Relocate(blob, resource);
    {
        void *drawable = p->draw50;
        int flags;
        flags = 0x2011;
        zero = 0;
        sub_0821983C(drawable, blob, 3, flags, zero, zero, 60, zero);
    }
    p->resource = Script_GetKeywordValue(114, 0);
    p->window = -1;
    p->state = zero;
    p->entered = 1;
    p->timer = zero;
    return 0;
failed:
    return -1;
}
