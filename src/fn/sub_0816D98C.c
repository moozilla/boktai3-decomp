#include "global.h"
#include "scene_hud_display.h"

s32 Inventory_GetQuantityByStorage(u32, u32);
u32 Selection_GetDisplayFrame(s32);
s32 SceneHud_CanApplySelectedAction(s32);
void PauseMenu_SetSpritePalette(struct SceneInputWidget *, u16);
void SceneHud_RefreshActionIcons(struct SceneHudState *owner)
{
    s32 i;
    u8 *category;
    u8 *base;
    u32 *flags;
    struct SceneInputWidget *w;
    i = 0;
    category = &owner->actionCategory;
    base = (u8 *)owner;
    flags = &owner->categoryWidgets[10].flags;
    w = &owner->categoryWidgets[10];
    for (; i < 16; base += sizeof(*w), w++, flags += sizeof(*w) / sizeof(*flags), i++) {
        s32 action = Inventory_GetQuantityByStorage(*category, i);
        if (action < 0) {
            *flags |= 1;
        } else {
            struct SceneInputWidget *current = ((struct SceneHudState *)base)->categoryWidgets + 10;
            current->flags &= ~1;
            Anim_LoadKeyframe(w, owner->motionResource, (u16)Selection_GetDisplayFrame(action), 1);
            if (*category == 0) {
                if ((u32)(action - 30) <= 1 || SceneHud_CanApplySelectedAction(action))
                    PauseMenu_SetSpritePalette(w, 0x373);
                else
                    PauseMenu_SetSpritePalette(w, 0x374);
            } else {
                PauseMenu_SetSpritePalette(w, 0x373);
            }
        }
    }
    owner = NULL;
}
