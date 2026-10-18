#define ScriptPanel_OpenTextWindow sub_08109C80
#include "global.h"
#include "engine.h"
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
s32 sub_08177D7C(void);
void Hud_RefreshSpriteFromScript(void);
s32 sub_08033690(s32, s32, s32, s32);
void Hud_SetMode(s32);
void TextWindow_StartPlayback(s32);
void sub_08215490(s32);
extern u32 gUnk_03005250, gUnk_0300523C, gUnk_03004DA0, gUnk_030051D4, gUnk_03005408;
extern u16 gUnk_03004BD8;
s32 sub_08109C80(struct ScriptPanel *p)
{
    if (Registry1745_ConsumeStateEntry(p)) {
        gUnk_03005250 = 0;
        {
            u32 mask = 4;
            gUnk_0300523C |= mask;
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
    gUnk_03004DA0 = 0x40;
    gUnk_030051D4 = 0x40;
    if (!p->hudReady) {
        if (sub_08177D7C()) {
            Hud_RefreshSpriteFromScript();
            p->hudReady = 1;
        }
    } else if (!p->windowReady && MessageWindow_IsCreated()) {
        p->window = sub_08033690(1, 6, 28, 6);
        p->windowReady = 1;
        Hud_SetMode(5);
        TextWindow_SetScript(p->window, p->script);
        TextWindow_Refresh(p->window, 1);
        TextWindow_StartPlayback(p->window);
        {
            u16 *flags = &gUnk_03004BD8;
            u32 mask = ~0xE00;
            *flags = mask & *flags;
        }
        sub_08215490(1);
    }
    if (p->counter <= 29)
        gUnk_03005408 = 1;
    return 0;
}
