#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
void PauseMenu_LoadAttributeIconFrame(void *, void *, s32);
void PauseMenu_LoadEquipmentIconFrame(void *, void *, s32);
void PauseMenu_LoadCategoryIconFrame(void *, void *, s32);
void PauseMenu_LoadStatusIconFrame(void *, void *, s32);
void PauseMenu_LoadFixedIconKeyframe(void *, void *);
void PauseMenu_LoadEmptySlotFrame(void *, void *);
static inline void show(u8 *p)
{
    *(u32 *)(p + 8) &= ~1;
}
static inline void hide(u8 *p)
{
    *(u32 *)(p + 8) |= 1;
}
void PauseMenu_UpdateSelectedEquipmentIcons(struct SceneHudState *p)
{
    u8 *owner = (u8 *)p->actionOwner;
    s8 *icon;
    u8 *entry;
    s32 i;
    if (owner[0x418] == 6) {
        PauseMenu_LoadAttributeIconFrame(p, p->primaryWidgets, 4);
        show((u8 *)p->secondaryWidgets);
        PauseMenu_LoadEquipmentIconFrame(p, p->secondaryWidgets, 13);
        show((u8 *)p->tertiaryWidgets);
        PauseMenu_LoadStatusIconFrame(p, p->tertiaryWidgets, 8);
        return;
    }
    if (owner[0x45F] == 1) {
        if (owner[0x418] == 2)
            PauseMenu_LoadAttributeIconFrame(p, p->primaryWidgets, 1);
        else if (owner[0x418] == 1)
            PauseMenu_LoadAttributeIconFrame(p, p->primaryWidgets, 2);
    } else if ((s8)owner[0x33C] >= 0) {
        icon = (s8 *)((u8 *)p + p->primarySelection * 4 + 0xC79);
        if (*icon > 5)
            PauseMenu_LoadAttributeIconFrame(p, p->primaryWidgets, *icon);
        else if (!owner[0x33D] || owner[0x340]) {
            if (p->primaryBlinkTimer == 0)
                PauseMenu_LoadFixedIconKeyframe(p, p->primaryWidgets);
            else if (p->primaryBlinkTimer == 30)
                PauseMenu_LoadAttributeIconFrame(p, p->primaryWidgets, *icon);
            if (++p->primaryBlinkTimer > 59)
                p->primaryBlinkTimer = 0;
        } else if (owner[0x33E]) {
            if ((u16)++p->actionTransferDelay & 1)
                PauseMenu_LoadAttributeIconFrame(
                    p, p->primaryWidgets, *(s8 *)((u8 *)p + (p->primarySelection << 2) + 0xC79));
            else
                PauseMenu_LoadEmptySlotFrame(p, p->primaryWidgets);
        } else
            PauseMenu_LoadAttributeIconFrame(p, p->primaryWidgets, *icon);
    }
    if (p->unknownF49) {
        PauseMenu_LoadEmptySlotFrame(p, p->secondaryWidgets);
        p->unknownF49--;
        return;
    }
    if ((u8)(p->actionOwner->actionMode - 1) <= 1) {
        hide((u8 *)p->secondaryWidgets);
        hide((u8 *)p->tertiaryWidgets);
        return;
    }
    entry = (u8 *)p + p->selection * 8;
    if (entry[0xDAA]) {
        PauseMenu_LoadEquipmentIconFrame(p, p->secondaryWidgets, entry[0xDAB]);
        show((u8 *)p->secondaryWidgets);
        show((u8 *)p->tertiaryWidgets);
        PauseMenu_LoadStatusIconFrame(p, p->tertiaryWidgets,
                                      p->tertiaryOptions[p->tertiarySelection].icon);
        for (i = 1; i <= 2; i++)
            p->tertiaryWidgets[i].flags |= 1;
    } else {
        PauseMenu_LoadCategoryIconFrame(p, p->secondaryWidgets, entry[0xDAB]);
        show((u8 *)p->secondaryWidgets);
        hide((u8 *)p->tertiaryWidgets);
    }
}
