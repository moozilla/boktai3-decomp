// Tentative StatusPanel family and semantic names; based on static consumers.
#define gStatusPanel gFieldHud
#include "global.h"
#include "ram.h"
struct PanelRefresh {
    u32 unknown[2];
    u32 flags;
};
struct StatusPanel {
    u8 unknown[0xA9C];
    struct PanelRefresh refresh;
};
void SelectionPanel_ClearTileColumns(void);
void SceneHud_ApplySecondarySelection(struct StatusPanel *);
void PauseMenu_ApplySelectedEquipment(struct StatusPanel *);
void SceneHud_RefreshWidgetVisibility(struct StatusPanel *);
void Hud_OnUnpausedRefresh(void);
void Hud_SetUpdateCallback(struct StatusPanel *, void (*)(void));
void StatusPanel_BeginRefresh(void)
{
    struct StatusPanel *p = gFieldHud;
    if (p) {
        struct PanelRefresh *refresh;
        SelectionPanel_ClearTileColumns();
        refresh = (struct PanelRefresh *)((u8 *)p + 0xA9C);
        refresh->flags |= 1;
        SceneHud_ApplySecondarySelection(p);
        PauseMenu_ApplySelectedEquipment(p);
        SceneHud_RefreshWidgetVisibility(p);
        Hud_SetUpdateCallback(p, Hud_OnUnpausedRefresh);
    }
}
