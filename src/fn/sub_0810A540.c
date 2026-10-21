#include "global.h"
#include "area_effect_link.h"

#ifdef NONMATCHING
// fuzzy: 97.5%
// open: the two Registry1745_SetState calls are not tail-merged; asm memory barrier forces the selection reload
#define LinkMenu_SetState       Registry1745_SetState
#define LinkMenu_CheckSelection Registry1745_CheckSessionReady
#define LinkMenu_ResetSelection Registry1745_BeginShutdown
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
void Script_Call(u32, void *);
void Sound_FadeOutChannel10(u32);
void ScriptPanel_OpenTextWindow(void);
void LinkMenu_ExchangeChoices(void);
void LinkMenu_WaitForReadyPeers(void);
static inline u8 IsMode9(u16 value)
{
    value &= 0x3c00;
    if (value == 0x1000)
        return 1;
    return 0;
}
static inline u8 IsMode4(u16 value)
{
    value &= 0x3c00;
    if (value == 0x400)
        return 1;
    return 0;
}
void RegistryF246_Close(void);
s32 LinkMenu_WaitSelectionMode(struct AreaEffectLinkState *p)
{
    void (*next)(void);
    u32 mode;
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (IsMode9(*p->selection)) {
        RegistryF246_Close();
        Registry1745_SetState(p, 9, LinkMenu_ExchangeChoices);
    } else {
        asm("" ::: "memory");
        if (IsMode4(*p->selection))
            Registry1745_SetState(p, 4, LinkMenu_WaitForReadyPeers);
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", LinkMenu_WaitSelectionMode);
#endif
