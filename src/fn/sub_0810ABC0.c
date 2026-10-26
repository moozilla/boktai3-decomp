#include "global.h"
#include "ram.h"
#define SessionMenu_ConsumeEntryFlag  Registry1745_ConsumeStateEntry
#define SessionMenu_ResetWithCallback Registry1745_BeginShutdown
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void Registry1745_OnStageIconComplete(void);
void StageIconDisplay_Create(u32, u32, void (*)(void), void *);
s32 SessionMenu_CreateStageIconDisplay(void *p)
{
    s32 result;
    gLinkMenuActive = 1;
    if (Registry1745_ConsumeStateEntry(p))
        StageIconDisplay_Create(0, 0, Registry1745_OnStageIconComplete, p);
    if (Registry1745_CheckSessionReady(p) >= 0) {
        result = 0;
    } else {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        result = -1;
    }
    return result;
}
