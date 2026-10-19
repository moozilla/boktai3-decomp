
#ifdef NONMATCHING
// fuzzy: 83.2%
// foundry draft (worker best 83.2%); what differs is mostly register allocation and instruction order
#define SceneHud_RefreshActionIcons sub_0816D98C
#define SceneHud_FindCategoryAction Inventory_GetQuantityByStorage
#define SceneHud_SetIconPalette PauseMenu_SetSpritePalette
#include "global.h"
#include "scene_hud_display.h"
s32 SceneHud_FindCategoryAction(u32, u32);
u32 sub_0822D604(s32);
s32 sub_0816D4E4(s32);
void SceneHud_SetIconPalette(struct SceneInputWidget *, u16);
void sub_0816D98C(u8 *s)
{
    struct SceneHudState *owner = (struct SceneHudState *)s;
    s32 i = 0;
    u8 *category = &owner->actionCategory;
    u8 *base = (u8 *)s;
    u32 *flags = (u32 *)(s + 0x17e8);
    struct SceneInputWidget *w = (struct SceneInputWidget *)(s + 0x17e0);
    for (; i < 16; base += 0x60, w++, flags += 0x18, i++) {
        s32 action = SceneHud_FindCategoryAction(*category, i);
        if (action < 0) {
            *flags |= 1;
        } else {
            struct SceneInputWidget *current = ((struct SceneHudState *)base)->categoryWidgets + 10;
            current->flags &= ~1;
            Anim_LoadKeyframe(w, owner->motionResource, (u16)sub_0822D604(action), 1);
            if (*category == 0) {
                if ((u32)(action - 30) <= 1 || sub_0816D4E4(action))
                    SceneHud_SetIconPalette(w, 0x373);
                else
                    SceneHud_SetIconPalette(w, 0x374);
            } else {
                SceneHud_SetIconPalette(w, 0x373);
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0816D98C);
#endif
