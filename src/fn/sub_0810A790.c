#include "global.h"
#define SessionMenu_SetState          Registry1745_SetState
#define SessionMenu_ConsumeEntryFlag  Registry1745_ConsumeStateEntry
#define SessionMenu_ResetWithCallback Registry1745_BeginShutdown
#include "area_effect_link.h"
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
void Script_Call(u32, void *);
void ScriptPanel_OpenTextWindow(void);
void LinkMenu_ShowReadyMessage(void);
s32 SessionMenu_RunEventScript(struct AreaEffectLinkState *p)
{
    if (Registry1745_ConsumeStateEntry(p))
        Script_Call(p->menuEventScript, 0);
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (p->menuEvent == 1)
        Registry1745_SetState(p, 12, LinkMenu_ShowReadyMessage);
    return 0;
}
