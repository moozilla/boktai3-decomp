// Tentative StatusPanel family and semantic names; based on static consumers.
#define StatusPanel_BeginRefresh sub_08169E94
#define gStatusPanel gUnk_02000214
#include "global.h"
struct PanelRefresh { u32 unknown[2]; u32 flags; };
struct StatusPanel {
    u8 unknown[0xA9C];
    struct PanelRefresh refresh;
};
extern struct StatusPanel *gUnk_02000214;
void sub_081681B8(void);
void sub_08168D20(struct StatusPanel *);
void PauseMenu_ApplySelectedEquipment(struct StatusPanel *);
void sub_081685B4(struct StatusPanel *);
void Hud_OnUnpausedRefresh(void);
void Hud_SetUpdateCallback(struct StatusPanel *, void (*)(void));
void sub_08169E94(void)
{
    struct StatusPanel *p = gUnk_02000214;
    if (p) {
        struct PanelRefresh *refresh;
        sub_081681B8();
        refresh = (struct PanelRefresh *)((u8 *)p + 0xA9C);
        refresh->flags |= 1;
        sub_08168D20(p);
        PauseMenu_ApplySelectedEquipment(p);
        sub_081685B4(p);
        Hud_SetUpdateCallback(p, Hud_OnUnpausedRefresh);
    }
}
