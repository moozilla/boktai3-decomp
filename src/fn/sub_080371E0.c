#include "global.h"
#include "render.h"
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
int WirelessStatusMenu_Update(struct LinkMenu *);
int WirelessStatusMenu_Destroy(struct LinkMenu *);
struct LinkMenu *WirelessStatusMenu_Get(void);
struct LinkMenu *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int WirelessStatusMenu_InitFromScript(struct LinkMenu *);
void Task_Kill(void *);
struct LinkMenu *WirelessStatusMenu_Create(void)
{
    struct LinkMenu *p = WirelessStatusMenu_Get();
    struct LinkMenu *result;
    if (p) {
        result = p;
        goto done;
    }
    {
        struct LinkMenu *allocated = Task_Create(12, 0xB0);
        void *update;
        void *destroy;
        p = allocated;
        if (!p)
            goto returnPointer;
        update = WirelessStatusMenu_Update;
        destroy = WirelessStatusMenu_Destroy;
        Task_SetCallbacks(allocated, update, destroy);
    }
    if (WirelessStatusMenu_InitFromScript(p) >= 0)
        goto returnPointer;
    Task_Kill(p);
    result = 0;
    goto done;
returnPointer:
    result = p;
done:
    return result;
}
