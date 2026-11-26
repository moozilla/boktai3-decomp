#include "global.h"
// confidence=low: refreshes inventory icons and equipped-slot markers.
struct PauseWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
    s16 x, y, z, pad;
    u8 unknown28[0x38];
};
struct SceneHud {
    u8 unknown0[0x4D90];
    u8 markers[16];
};
void Anim_LoadKeyframe(void *, void *, u16, u32);
void PauseMenu_SetSpritePalette(void *, u32);
void PauseMenu_ClearSpriteFlags(void *, u32, u32);
void PauseMenu_AddSpriteFlags(void *, u32, u32);
void PauseMenu_SetSpritePosition(void *, u32, s32, s32);
s32 Inventory_GetEquipmentId(u32);
s32 ItemRecord_MeetsLevelRequirement(u32);
s32 Inventory_CanAssignEquipment(u32);
s32 Inventory_IsEquipmentAssigned(u32);
static inline void SceneHud_ClearMarkerSlots(s32 j, u8 *slots, u32 mask)
{
    do {
        u32 value = slots[j];
        value |= mask;
        slots[j] = value;
        j++;
    } while (j < 8);
}
void SceneHud_RefreshEquipmentIcons(u8 *p, u32 mode)
{
    s32 i;
    s32 markers;
    u32 offset;
    struct PauseWidget *widget;
    u8 *flagBase;
    SceneHud_ClearMarkerSlots(0, p + 0x4D90, 255);
    markers = 0;
    i = 0;
    widget = (void *)(p + 0x17E0);
    offset = 0;
    flagBase = p;
    do {
        s32 id = Inventory_GetEquipmentId(i);
        if (id == 255)
            widget->flags |= 1;
        else {
            u32 widgetOffset = 0x17E0;
            ((struct PauseWidget *)(flagBase + widgetOffset))->flags &= ~1;
            widgetOffset += offset;
            Anim_LoadKeyframe(p + widgetOffset, p + 0xA4, (u16)(id + 0xF0), 0);
            if (((mode == 0) && (!ItemRecord_MeetsLevelRequirement(i))) ||
                ((mode == 1) && (!Inventory_CanAssignEquipment(i))))
                PauseMenu_SetSpritePalette(widget, 0x374);
            else
                PauseMenu_SetSpritePalette(widget, 0x373);
            if (Inventory_IsEquipmentAssigned(i)) {
                s32 marker = markers + 30;
                PauseMenu_ClearSpriteFlags(p, marker, 1);
                PauseMenu_SetSpritePosition(p, marker, widget->x, widget->y);
                ((struct SceneHud *)p)->markers[markers] = i;
                markers++;
            }
        }
        widget++;
        offset += 0x60;
        flagBase += 0x60;
        i++;
    } while (i <= 15);
    for (; markers <= 15; markers++)
        PauseMenu_AddSpriteFlags(p, markers + 30, 1);
}
