#define LinkMenu_ConsumeStateEntry Registry1745_ConsumeStateEntry
#define LinkMenu_UpdateSelection   sub_0810A448
#define LinkMenu_SetState Registry1745_SetState
#define LinkMenu_CheckSession Registry1745_CheckSessionReady
#define LinkMenu_ResetSelection Registry1745_BeginShutdown
#define gLinkMenuState             gUnk_020004B4
#include "global.h"
#include "area_effect_link.h"
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
void Script_Call(u32, void *);
void Sound_FadeOutChannel10(u32);
void sub_08109C80(void);
void sub_0810A5C0(void);
void sub_08109F1C(void);
static inline u8 IsMode9(u16 *p)
{
    u16 value = *p;
    value &= 0x3c00;
    if (value == 0x1000)
        return 1;
    return 0;
}
static inline u8 IsMode4(u16 *p)
{
    u16 value = *p;
    value &= 0x3c00;
    if (value == 0x400)
        return 1;
    return 0;
}
s32 sub_0810A448(struct AreaEffectLinkState *p)
{
    if (Registry1745_ConsumeStateEntry(p))
        Script_Call(p->selectionScript, 0);
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, sub_08109C80);
        return -1;
    }
    if (IsMode9(p->selection))
        Registry1745_SetState(p, 9, sub_0810A5C0);
    else if (IsMode4(p->selection)) {
        Registry1745_SetState(p, 4, sub_08109F1C);
        if (gUnk_020004B4 && gUnk_020004B4->localPlayer == 0)
            Sound_FadeOutChannel10(1);
    } else if (gUnk_020004B4 && gUnk_020004B4->localPlayer == 0) {
        u32 value = p->menuEvent;
        u8 *event = &p->menuEvent;
        if (value) {
            if (value == 1)
                p->requestedMode = 0x1000;
            else if (value == 2) {
                p->requestedMode = 0x400;
                Script_Call(p->confirmScript, 0);
            }
            *event = 0;
        }
    }
    return 0;
}
