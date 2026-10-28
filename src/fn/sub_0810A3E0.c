#include "global.h"
#define SessionMenu_SetState          Registry1745_SetState
#define SessionMenu_ResetWithCallback Registry1745_BeginShutdown
#include "area_effect_link.h"
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void LinkMenu_UpdateSelection(void);
void LinkMenu_WaitSelectionMode(void);
void RegistryF246_Close(void);
s32 SessionMenu_WaitForSelection(struct AreaEffectLinkState *p)
{
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (p->menuElapsed > 120) {
        if (gLinkSession && gLinkSession->localPlayer == 0) {
            Registry1745_SetState(p, 7, LinkMenu_UpdateSelection);
            RegistryF246_Close();
        } else {
            Registry1745_SetState(p, 8, LinkMenu_WaitSelectionMode);
        }
    }
    return 0;
}
