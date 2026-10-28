#define LinkMenu_ConsumeStateEntry Registry1745_ConsumeStateEntry
#define LinkMenu_SetState          Registry1745_SetState
#define LinkMenu_CheckSession      Registry1745_CheckSessionReady
#define LinkMenu_ResetSelection    Registry1745_BeginShutdown
#include "global.h"
#include "area_effect_link.h"
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
void Script_Call(u32, void *);
void Sound_FadeOutChannel10(u32);
void ScriptPanel_OpenTextWindow(void);
void LinkMenu_ExchangeChoices(void);
void LinkMenu_WaitForReadyPeers(void);
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
s32 LinkMenu_UpdateSelection(struct AreaEffectLinkState *p)
{
    if (Registry1745_ConsumeStateEntry(p))
        Script_Call(p->selectionScript, 0);
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (IsMode9(p->selection))
        Registry1745_SetState(p, 9, LinkMenu_ExchangeChoices);
    else if (IsMode4(p->selection)) {
        Registry1745_SetState(p, 4, LinkMenu_WaitForReadyPeers);
        if (gLinkSession && gLinkSession->localPlayer == 0)
            Sound_FadeOutChannel10(1);
    } else if (gLinkSession && gLinkSession->localPlayer == 0) {
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
