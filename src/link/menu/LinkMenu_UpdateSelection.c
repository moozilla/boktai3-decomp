#define LinkMenu_ConsumeStateEntry LinkSession_ConsumeStateEntry
#define LinkMenu_SetState          LinkSession_SetState
#define LinkMenu_CheckSession      LinkSession_CheckSessionReady
#define LinkMenu_ResetSelection    LinkSession_BeginShutdown
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
    if (LinkSession_ConsumeStateEntry(p))
        Script_Call(p->selectionScript, 0);
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (IsMode9(p->selection))
        LinkSession_SetState(p, 9, LinkMenu_ExchangeChoices);
    else if (IsMode4(p->selection)) {
        LinkSession_SetState(p, 4, LinkMenu_WaitForReadyPeers);
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
