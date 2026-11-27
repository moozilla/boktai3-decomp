#include "global.h"
#define SessionMenu_SetState          LinkSession_SetState
#define SessionMenu_ResetWithCallback LinkSession_BeginShutdown
#include "scene/areas/area_effect_link.h"
s32 LinkSession_CheckSessionReady(void *);
void LinkSession_BeginShutdown(void *, void (*)(void));
void LinkSession_SetState(void *, u32, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void LinkMenu_UpdateSelection(void);
void LinkMenu_WaitSelectionMode(void);
void LinkPanelSlots_Close(void);
s32 SessionMenu_WaitForSelection(struct AreaEffectLinkState *p)
{
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (p->menuElapsed > 120) {
        if (gLinkSession && gLinkSession->localPlayer == 0) {
            LinkSession_SetState(p, 7, LinkMenu_UpdateSelection);
            LinkPanelSlots_Close();
        } else {
            LinkSession_SetState(p, 8, LinkMenu_WaitSelectionMode);
        }
    }
    return 0;
}
