#include "global.h"
#define SessionMenu_SetState          LinkSession_SetState
#define SessionMenu_ConsumeEntryFlag  LinkSession_ConsumeStateEntry
#define SessionMenu_ResetWithCallback LinkSession_BeginShutdown
#include "scene/areas/area_effect_link.h"
#include "engine/engine.h"
s32 TextWindow_Create(s32, s32, s32, s32);
void TextWindow_StartPlayback(s32);
u8 LinkSession_ConsumeStateEntry(void *);
s32 LinkSession_CheckSessionReady(void *);
void LinkSession_BeginShutdown(void *, void (*)(void));
void LinkSession_SetState(void *, u32, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void SceneMenu_WaitForAcknowledgements(void);
s32 SessionMenu_ShowTimedMessage(struct AreaEffectLinkState *p)
{
    if (LinkSession_ConsumeStateEntry(p)) {
        p->menuTextWindow = TextWindow_Create(1, 6, 28, 6);
        TextWindow_SetScript(p->menuTextWindow, p->menuMessageScript);
        TextWindow_Refresh(p->menuTextWindow, 10);
        TextWindow_StartPlayback(p->menuTextWindow);
    }
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (p->menuElapsed > 119)
        LinkSession_SetState(p, 24, SceneMenu_WaitForAcknowledgements);
    /* Callback's success path leaves the machine return register unchanged. */
}
