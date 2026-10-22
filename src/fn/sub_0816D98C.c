#include "global.h"
#include "scene_hud_display.h"

#ifdef NONMATCHING
// fuzzy: 97.9%
// only the base copy (mov r7, r8) is placed after the flags/widget pointers; assigning it first makes CSE derive them from the copy
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
    flags = (u32 *)((u8 *)owner + 0x17e8);
    w = (struct SceneInputWidget *)((u8 *)owner + 0x17e0);
    base = (u8 *)owner;
    for (; i < 16; base += 0x60, w++, flags += 0x18, i++) {
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
}
#else
INCLUDE_ASM("asm/nonmatching", SceneHud_RefreshActionIcons);
#endif
