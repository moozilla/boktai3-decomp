#include "global.h"
#include "render.h"
#include "ram.h"
#include "input.h"
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
int Script_SeekToKeyword(int);
void *Script_GetTextSourcePc(void);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, int, int, int, int, int, int);
void *Script_GetKeywordValue(int, int);
int WirelessStatusMenu_InitFromScript(struct LinkMenu *p)
{
    struct Blob *resource;
    struct Blob *blob;
    int zero;
    gLinkMenu = p;
    PaletteLevels_Reset();
    if (!Script_SeekToKeyword(115))
        goto failed;
    p->script = Script_GetTextSourcePc();
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
        Widget_InitLinked(drawable, blob, 3, flags, zero, zero, 60, zero);
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
