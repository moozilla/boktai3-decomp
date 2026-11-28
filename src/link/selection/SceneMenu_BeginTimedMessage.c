#include "global.h"
#include "scene/areas/area_effect_link.h"
u8 LinkSession_ConsumeStateEntry(struct AreaEffectLinkState *);
s32 LinkSession_CheckSessionReady(struct AreaEffectLinkState *);
void LinkSession_BeginShutdown(struct AreaEffectLinkState *, void (*)(void));
void LinkSession_SetState(struct AreaEffectLinkState *, u32, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void SceneMenu_UpdateRecords(void);
s32 TextWindow_Create(s32, s32, s32, s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
s32 SceneMenu_BeginTimedMessage(struct AreaEffectLinkState *p)
{
    if (LinkSession_ConsumeStateEntry(p)) {
        p->exchangeElapsed = 0;
        p->exchangeFrames = 0;
        if (gLinkSession)
            gLinkSession->unknown52 = 1;
        p->menuTextWindow = TextWindow_Create(1, 6, 28, 6);
        TextWindow_SetScript(p->menuTextWindow, p->menuMessageScript);
        TextWindow_Refresh(p->menuTextWindow, 8);
        TextWindow_StartPlayback(p->menuTextWindow);
    }
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (p->menuElapsed <= 59)
        LinkSession_SetState(p, 0x16, SceneMenu_UpdateRecords);
    return 0;
}
