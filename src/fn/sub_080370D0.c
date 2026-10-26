#include "global.h"
#include "render.h"
#include "ram.h"
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
extern u16 gUnk_03005260[];
extern u16 gUnk_03004FC0[];
void TextWindowManager_DestroyAllWindows(void);
void PaletteLevels_Reset(void);
void SpriteOam_ResetQueue(int);
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
void WirelessStatusMenu_InitDisplay(void);
void WirelessStatusMenu_LoadStatusBackground(struct LinkMenu *, int);
int TextWindow_Create(int, int, int, int);
void TextWindow_SetScript(int, void *);
void TextWindow_Refresh(int, int);
void TextWindow_StartPlayback(int);
void TextWindow_Destroy(int);
void Sound_PlaySong(int);
int Wireless_GetStatus(void);
int Callback_InvokeActor(void *, void *);
extern void *gUnk_08605770[];
void Anim_Remove(void *);
int WirelessStatusMenu_Update(struct LinkMenu *p)
{
    int previous = p->state;
    if (!previous) {
        int state = Wireless_GetStatus();
        if (state == 1) {
            p->state = state;
            p->entered = state;
            p->timer = previous;
        } else if (state == 2) {
            p->state = state;
            p->entered = 1;
            p->timer = previous;
        }
    }
    Callback_InvokeActor(p, gUnk_08605770[p->state]);
    return 0;
}