#include "global.h"
#include "engine/ram.h"
u8 LinkSession_ConsumeStateEntry(void *);
s32 LinkSession_CheckSessionReady(void *);
void LinkSession_BeginShutdown(void *, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void LinkSession_OnStageIconComplete(void);
void StageIconDisplay_Create(u32, u32, void (*)(void), void *);
s32 SessionMenu_CreateStageIconDisplay(void *p)
{
    gLinkMenuActive = 1;
    if (LinkSession_ConsumeStateEntry(p))
        StageIconDisplay_Create(0, 0, LinkSession_OnStageIconComplete, p);
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    return 0;
}
