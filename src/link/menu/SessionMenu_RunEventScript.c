#include "global.h"
#define SessionMenu_SetState          LinkSession_SetState
#define SessionMenu_ConsumeEntryFlag  LinkSession_ConsumeStateEntry
#define SessionMenu_ResetWithCallback LinkSession_BeginShutdown
#include "scene/areas/area_effect_link.h"
u8 LinkSession_ConsumeStateEntry(void *);
s32 LinkSession_CheckSessionReady(void *);
void LinkSession_BeginShutdown(void *, void (*)(void));
void LinkSession_SetState(void *, u32, void (*)(void));
void Script_Call(u32, void *);
void ScriptPanel_OpenTextWindow(void);
void LinkMenu_ShowReadyMessage(void);
s32 SessionMenu_RunEventScript(struct AreaEffectLinkState *p)
{
    if (LinkSession_ConsumeStateEntry(p))
        Script_Call(p->menuEventScript, 0);
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (p->menuEvent == 1)
        LinkSession_SetState(p, 12, LinkMenu_ShowReadyMessage);
    return 0;
}
