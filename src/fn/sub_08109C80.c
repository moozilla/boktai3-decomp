#include "global.h"
#include "engine.h"
#include "render.h"
struct ScriptPanel {
    u8 unk0[0x30];
    u32 counter;
    u8 unk34[0xA];
    u16 scale;
    u8 unk40[0x11];
    u8 windowReady;
    u8 unk52[0x26];
    u8 hudReady;
    u8 unk79[0x3F];
    s32 window;
    u8 unkBC[0x1C];
    const u8 *script;
};
u8 Registry1745_ConsumeStateEntry(struct ScriptPanel *);
u8 MessageWindow_IsCreated(void);
void Sound_FadeOutChannel10(u32);
void ScrollingDialogue_HideImmediately(void);
void Video_OnFrameNoOp(void);
void Video_OnRenderNoOp(void);
void Sprite_OnRenderPhase(void);
void Video_SetRenderCallbacks(s32, void *, void *, void *);
void Sound_StopTrackedSong(u32);
void Sound_StopAllTrackedSongs(void);
s32 Hud_GetManager(void);
void Hud_RefreshSpriteFromScript(void);
s32 TextWindow_Create(s32, s32, s32, s32);
void Hud_SetMode(s32);
void TextWindow_StartPlayback(s32);
void SpriteOam_ResetQueue(s32);
extern u32 gUnk_03005250, gUnk_03005408;
s32 ScriptPanel_OpenTextWindow(struct ScriptPanel *p)
{
    if (Registry1745_ConsumeStateEntry(p)) {
        gUnk_03005250 = 0;
        {
            u32 mask = 4;
            gEnginePauseFlags |= mask;
        }
        p->windowReady = 0;
        p->hudReady = 0;
        p->scale = 0x4000;
        Sound_FadeOutChannel10(1);
        ScrollingDialogue_HideImmediately();
        Bg_ClearTilemap(0);
        Video_SetRenderCallbacks(0, Video_OnFrameNoOp, Video_OnRenderNoOp, Sprite_OnRenderPhase);
        Sound_StopTrackedSong(0x33);
        Sound_StopAllTrackedSongs();
    }
    gBgFadeLevel = 0x40;
    gObjFadeLevel = 0x40;
    if (!p->hudReady) {
        if (Hud_GetManager()) {
            Hud_RefreshSpriteFromScript();
            p->hudReady = 1;
        }
    } else if (!p->windowReady && MessageWindow_IsCreated()) {
        p->window = TextWindow_Create(1, 6, 28, 6);
        p->windowReady = 1;
        Hud_SetMode(5);
        TextWindow_SetScript(p->window, p->script);
        TextWindow_Refresh(p->window, 1);
        TextWindow_StartPlayback(p->window);
        {
            u16 *flags = &gDispCntShadow;
            u32 mask = ~0xE00;
            *flags = mask & *flags;
        }
        SpriteOam_ResetQueue(1);
    }
    if (p->counter <= 29)
        gUnk_03005408 = 1;
    return 0;
}
