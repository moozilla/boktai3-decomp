#include "global.h"
#include "scene/areas/area_effect_link.h"
#include "link/selection/link_selection_mode.h"
s32 LinkSession_CheckSessionReady(void *);
void LinkSession_BeginShutdown(void *, void (*)(void));
void LinkSession_SetState(void *, u32, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void LinkMenu_ExchangeChoices(void);
void LinkMenu_WaitForReadyPeers(void);
void LinkPanelSlots_Close(void);
static inline u8 IsWaitForPeersMode(u16 value)
{
    value &= 0x3c00;
    if (value == 0x400)
        return 1;
    return 0;
}
/* Reading through the session handle keeps the second test from reusing this load. */
static inline u8 IsExchangeChoicesMode(struct AreaEffectLinkState **session)
{
    u16 value = *(*session)->selection;
    value &= 0x3c00;
    if (value == 0x1000)
        return 1;
    return 0;
}
s32 LinkMenu_WaitSelectionMode(struct AreaEffectLinkState *p)
{
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (IsExchangeChoicesMode(&p)) {
        LinkPanelSlots_Close();
        LinkSession_SetState(p, 9, LinkMenu_ExchangeChoices);
        return 0;
    }
    if (IsWaitForPeersMode(*p->selection)) {
        LinkSession_SetState(p, 4, LinkMenu_WaitForReadyPeers);
        return 0;
    }
    return 0;
}
