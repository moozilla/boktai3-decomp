#include "global.h"
#define SessionMenu_SetState Registry1745_SetState
#define SessionMenu_ConsumeEntryFlag Registry1745_ConsumeStateEntry
#define SessionMenu_ResetWithCallback Registry1745_BeginShutdown
#include "area_effect_link.h"
#include "engine.h"
s32 sub_08033690(s32, s32, s32, s32);
void TextWindow_StartPlayback(s32);
#define SessionMenu_ShowTimedMessage sub_0810B4C0
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
void sub_08109C80(void);
void sub_0810B534(void);
s32 sub_0810B4C0(struct AreaEffectLinkState *p)
{
    if (Registry1745_ConsumeStateEntry(p)) {
        p->menuTextWindow = sub_08033690(1, 6, 28, 6);
        TextWindow_SetScript(p->menuTextWindow, p->menuMessageScript);
        TextWindow_Refresh(p->menuTextWindow, 10);
        TextWindow_StartPlayback(p->menuTextWindow);
    }
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, sub_08109C80);
        return -1;
    }
    if (p->menuElapsed > 119)
        Registry1745_SetState(p, 24, sub_0810B534);
    /* Callback's success path leaves the machine return register unchanged. */
}
