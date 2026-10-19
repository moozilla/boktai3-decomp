#include "global.h"
#include "area_effect_link.h"

#ifdef NONMATCHING
// fuzzy: 80.7%
// foundry draft (worker best 80.7%); what differs is mostly register allocation and instruction order
#define LinkMenu_WaitSelectionMode sub_0810A540
#define LinkMenu_SetState          Registry1745_SetState
#define LinkMenu_CheckSelection    Registry1745_CheckSessionReady
#define LinkMenu_ResetSelection    Registry1745_BeginShutdown
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
void Script_Call(u32, void *);
void Sound_FadeOutChannel10(u32);
void ScriptPanel_OpenTextWindow(void);
void LinkMenu_ExchangeChoices(void);
void sub_08109F1C(void);
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
s32 sub_0810A540(struct AreaEffectLinkState *p)
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
        return 0;
    }
    if (IsMode4(*p->selection))
        Registry1745_SetState(p, 4, sub_08109F1C);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0810A540);
#endif
