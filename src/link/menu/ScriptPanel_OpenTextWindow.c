#include "global.h"
#include "engine/engine.h"
#include "engine/video/render.h"
#include "gba/io_reg.h"
#include "engine/ram.h"
#include "engine/runtime.h"
#include "engine/engine_pause_flags.h"
#include "link/link_script_panel.h"
u8 LinkSession_ConsumeStateEntry(struct LinkScriptPanel *);
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
s32 ScriptPanel_OpenTextWindow(struct LinkScriptPanel *p)
{
    if (LinkSession_ConsumeStateEntry(p)) {
        gLinkMenuActive = 0;
        Engine_SetPauseFlags(4);
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
        TextWindow_SetScript(p->window, p->scriptD8);
        TextWindow_Refresh(p->window, 1);
        TextWindow_StartPlayback(p->window);
        Bg_DisableDisplay(DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_BG3_ON);
        SpriteOam_ResetQueue(1);
    }
    if (p->counter <= 29)
        gSceneDialogActive = 1;
    return 0;
}
