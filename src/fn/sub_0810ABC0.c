#include "global.h"
#define SessionMenu_ConsumeEntryFlag Registry1745_ConsumeStateEntry
#define SessionMenu_ResetWithCallback Registry1745_BeginShutdown
#define SessionMenu_CreateStageIconDisplay sub_0810ABC0
extern u32 gUnk_03005250;
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void sub_08109C80(void);
void Registry1745_OnStageIconComplete(void);
void StageIconDisplay_Create(u32, u32, void (*)(void), void *);
s32 sub_0810ABC0(void *p)
{
    s32 result;
    gUnk_03005250 = 1;
    if (Registry1745_ConsumeStateEntry(p))
        StageIconDisplay_Create(0, 0, Registry1745_OnStageIconComplete, p);
    if (Registry1745_CheckSessionReady(p) >= 0) {
        result = 0;
    } else {
        Registry1745_BeginShutdown(p, sub_08109C80);
        result = -1;
    }
    return result;
}
