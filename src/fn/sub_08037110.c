// MATCHING_CONCESSION: register-pinning
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
void TextWindowManager_DestroyAllWindows(void);
void PaletteLevels_Reset(void);
void sub_08215490(int);
void Video_OnFrameNoOp(void);
void Video_OnRenderNoOp(void);
void sub_0821969C(void);
void Video_SetRenderCallbacks(int, void *, void *, void *);
void Hud_SetMode(int);
void Bg_ClearTilemap(int);
void *ScriptProc_Start(int, int);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void BgGfx_LoadTilemapSimple(int, int, int, int, int);
void Wireless_RequestDisconnectMode(void);
void Sound_FadeOutChannel10(int);
void WirelessStatusMenu_InitDisplay(struct LinkMenu *);
void WirelessStatusMenu_LoadStatusBackground(struct LinkMenu *, int);
int sub_08033690(int, int, int, int);
void TextWindow_SetScript(int, void *);
void TextWindow_Refresh(int, int);
void TextWindow_StartPlayback(int);
void TextWindow_Destroy(int);
void Sound_PlaySong(int);
int Wireless_GetStatus(void);
int sub_0824923C(void *, void *);
extern void *gUnk_08605770[];
void Anim_Remove(void *);
int WirelessStatusMenu_Destroy(struct LinkMenu *p)
{
    TextWindow_Destroy(p->window);
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(2);
    Anim_Remove(p->draw50);
    {
        struct LinkMenu **singleton = &gUnk_020000F8;
        register u32 zero asm("r0");
        zero = 0;
        *singleton = (void *)zero;
    }
}