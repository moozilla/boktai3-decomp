#define SceneHud_IsSecondaryEntryEnabled PauseMenu_GetSelectedListEntryType
#define SceneHud_TestOwnerModeGate PauseMenu_IsEquipmentSelectionEnabled
#define SceneHud_RefreshWidgetVisibility sub_081685B4
#include "global.h"
#include "scene_hud_display.h"
s32 PauseMenu_IsEquipmentSelectionEnabled(struct SceneHudState *);
u32 PauseMenu_GetSelectedListEntryType(struct SceneHudState *);
void sub_081685B4(struct SceneHudState *p)
{
    s32 blocked = 0;
    s32 i;
    if (PauseMenu_IsEquipmentSelectionEnabled(p) == 1)
        blocked = 1;
    i = 0;
    {
        u32 mask = ~1;
        u32 one = 1;
        u32 *cf = &p->tertiaryWidgets[0].flags;
        u32 *bf = &p->secondaryWidgets[0].flags;
        u32 *af = &p->primaryWidgets[0].flags;
        for (; i <= 2; i++) {
            if (i == 0 && p->primaryVisible)
                ((struct SceneInputWidget *)((u8 *)p + 0xC88))->flags &= mask;
            else
                *af |= one;
            if (i == 0 && p->secondaryCount && !blocked)
                ((struct SceneInputWidget *)((u8 *)p + 0xE28))->flags &= mask;
            else
                *bf |= one;
            if (PauseMenu_GetSelectedListEntryType(p) && i == 0 && p->tertiaryVisible && !blocked)
                ((struct SceneInputWidget *)((u8 *)p + 0xF70))->flags &= mask;
            else
                *cf |= one;
            cf += 24;
            bf += 24;
            af += 24;
        }
    }
    {
        u32 one = 1;
        u32 *flags = &p->extraWidgetsA[0].flags;
        u8 *q = (u8 *)p;
        u32 offsetB = 0x11B0;
        u32 offsetC = 0x12D0;
        for (i = 2; i >= 0; i--) {
            *flags |= one;
            ((struct SceneInputWidget *)(q + offsetB))->flags |= one;
            ((struct SceneInputWidget *)(q + offsetC))->flags |= one;
            flags += 24;
            q += 0x60;
        }
    }
}
