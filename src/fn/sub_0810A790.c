#include "global.h"
#define SessionMenu_SetState Registry1745_SetState
#define SessionMenu_ConsumeEntryFlag Registry1745_ConsumeStateEntry
#define SessionMenu_ResetWithCallback Registry1745_BeginShutdown
#include "area_effect_link.h"
#define SessionMenu_RunEventScript sub_0810A790
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
void Script_Call(u32, void *);
void sub_08109C80(void);
void sub_0810A7E8(void);
s32 sub_0810A790(struct AreaEffectLinkState *p)
{
    if (Registry1745_ConsumeStateEntry(p))
        Script_Call(p->menuEventScript, 0);
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, sub_08109C80);
        return -1;
    }
    if (p->menuEvent == 1)
        Registry1745_SetState(p, 12, sub_0810A7E8);
    return 0;
}
