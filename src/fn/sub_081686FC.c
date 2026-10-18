#define SceneHud_UpdateSecondaryWidgets sub_081686FC
#define SceneHud_LoadInactiveIcon PauseMenu_LoadCategoryIconFrame
#define SceneHud_LoadActiveIcon PauseMenu_LoadEquipmentIconFrame
#define SceneHud_GetCarouselY PauseMenu_GetEquipmentRowY
#include "global.h"
#include "scene_hud_display.h"
void PauseMenu_LoadCategoryIconFrame(struct SceneHudState *, struct SceneInputWidget *, s32);
void PauseMenu_LoadEquipmentIconFrame(struct SceneHudState *, struct SceneInputWidget *, s32);
s32 PauseMenu_GetEquipmentRowY(struct SceneHudState *, s32, s32, s32, s32);
void sub_081686FC(struct SceneHudState *p)
{
    s32 i = 0;
    struct SceneInputWidget *w = (struct SceneInputWidget *)((u8 *)p + 0xE28);
    for (; i <= 2; w++, i++) {
        s32 index = i + p->selection;
        struct SceneHudState *entry;
        if (index >= p->secondaryCount)
            index -= p->secondaryCount;
        entry = (struct SceneHudState *)((u8 *)p + index * 8);
        if (entry->secondaryEntryEnabled)
            PauseMenu_LoadEquipmentIconFrame(p, w, entry->secondaryEntryIcon);
        else
            PauseMenu_LoadCategoryIconFrame(p, w, entry->secondaryEntryIcon);
        w->position.y = PauseMenu_GetEquipmentRowY(p, i, p->selection, 3, 0);
    }
}
