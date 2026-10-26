#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
#include "game_state.h"

struct EquipmentDescriptor {
    u8 unk_00[2];
    u8 requiredLevel;
    u8 unk_03[29];
};
extern const struct EquipmentDescriptor gUnk_08E6092C[];
s32 Inventory_GetEquipmentId(u32);
void PauseMenu_SetSpritePalette(struct SceneInputWidget *, u16);

/* Tentative: refresh equipment icons, with a fixed icon in alternate action mode. */
#ifdef NONMATCHING
// fuzzy: 62.7%
// scope shape 151 (differences once low registers are set aside)
void sub_08174930(struct SceneHudState *owner)
{
    s32 i;
    u32 *flags;
    u32 mask;

    if (owner->actionOwner->actionMode == 6) {
        struct SceneInputWidget *widget;
        owner->categoryWidgets[10].flags &= ~1;
        widget = &owner->categoryWidgets[10];
        Anim_LoadKeyframe(widget, owner->motionResource, 0x2F, 1);
        PauseMenu_SetSpritePalette(widget, 0x373);
        mask = 1;
        flags = &owner->categoryWidgets[11].flags;
        i = 14;
        do {
            *flags |= mask;
            flags += sizeof(*widget) / sizeof(*flags);
        } while (--i >= 0);
    } else {
        struct SceneInputWidget *widget;
        i = 0;
        widget = &owner->categoryWidgets[10];
        do {
            s32 id = Inventory_GetEquipmentId(i);
            if (id == 255) {
                owner->categoryWidgets[i + 10].flags |= 1;
            } else {
                owner->categoryWidgets[i + 10].flags &= ~1;
                Anim_LoadKeyframe(widget, owner->motionResource, (u16)id, 1);
                PauseMenu_SetSpritePalette(widget, 0x373);
                if (gGameState->level >= gUnk_08E6092C[id].requiredLevel)
                    PauseMenu_SetSpritePalette(widget, 0x373);
                else
                    PauseMenu_SetSpritePalette(widget, 0x374);
            }
            widget++;
            i++;
        } while (i <= 15);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08174930);
#endif
