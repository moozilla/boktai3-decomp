#include "global.h"
#define SessionMenu_SetState Registry1745_SetState
#define SessionMenu_ConsumeEntryFlag Registry1745_ConsumeStateEntry
#define SessionMenu_ResetWithCallback Registry1745_BeginShutdown
#include "area_effect_link.h"
#include "engine.h"
s32 TextWindow_Create(s32, s32, s32, s32);
void TextWindow_StartPlayback(s32);
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void SceneMenu_WaitForAcknowledgements(void);
s32 SessionMenu_ShowTimedMessage(struct AreaEffectLinkState *p)
{
    if (Registry1745_ConsumeStateEntry(p)) {
        p->menuTextWindow = TextWindow_Create(1, 6, 28, 6);
        TextWindow_SetScript(p->menuTextWindow, p->menuMessageScript);
        TextWindow_Refresh(p->menuTextWindow, 10);
        TextWindow_StartPlayback(p->menuTextWindow);
    }
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (p->menuElapsed > 119)
        Registry1745_SetState(p, 24, SceneMenu_WaitForAcknowledgements);
    /* Callback's success path leaves the machine return register unchanged. */
}
