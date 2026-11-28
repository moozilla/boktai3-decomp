#include "global.h"
#include "scene/areas/area_effect_link.h"
u8 LinkSession_ConsumeStateEntry(void *);
s32 LinkSession_CheckSessionReady(void *);
void LinkSession_BeginShutdown(void *, void (*)(void));
void LinkSession_SetState(void *, u32, void (*)(void));
void Script_Call(u32, void *);
void Sound_FadeOutChannel10(u32);
void ScriptPanel_OpenTextWindow(void);
void LinkMenu_ExchangeChoices(void);
void LinkMenu_WaitForReadyPeers(void);
static inline u8 IsExchangeChoicesMode(u16 value)
{
    value &= 0x3c00;
    if (value == 0x1000)
        return 1;
    return 0;
}
static inline u8 IsWaitForPeersMode(u16 value)
{
    value &= 0x3c00;
    if (value == 0x400)
        return 1;
    return 0;
}
s32 LinkMenu_UpdateSelection(struct AreaEffectLinkState *p)
{
    if (LinkSession_ConsumeStateEntry(p))
        Script_Call(p->selectionScript, NULL);
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (IsExchangeChoicesMode(*p->selection))
        LinkSession_SetState(p, 9, LinkMenu_ExchangeChoices);
    else if (IsWaitForPeersMode(*p->selection)) {
        LinkSession_SetState(p, 4, LinkMenu_WaitForReadyPeers);
        if (gLinkSession && gLinkSession->localPlayer == 0)
            Sound_FadeOutChannel10(1);
    } else if (gLinkSession && gLinkSession->localPlayer == 0) {
        if (p->menuEvent) {
            if (p->menuEvent == 1) {
                p->requestedMode = 0x1000;
            } else if (p->menuEvent == 2) {
                p->requestedMode = 0x400;
                Script_Call(p->confirmScript, NULL);
            }
            p->menuEvent = 0;
        }
    }
    return 0;
}
