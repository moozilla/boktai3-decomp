#include "global.h"
// confidence=low: refreshes inventory icons and equipped-slot markers.
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining: loop induction, marker pointers and register allocation.
struct PauseWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
    s16 x, y, z, pad;
    u8 unknown28[0x38];
};
extern u8 *gUnk_02000710;
s32 Inventory_GetConsumableId(u32);
s32 GameState_IsConsumableSlotEquipped(u32);
void Anim_LoadKeyframe(void *, void *, u16, u32);
void PauseMenu_SetSpritePalette(void *, u32);
void PauseMenu_ClearSpriteFlags(void *, u32, u32);
void PauseMenu_AddSpriteFlags(void *, u32, u32);
void PauseMenu_SetSpritePosition(void *, u32, s32, s32);
s32 Inventory_GetEquipmentId(u32);
s32 ItemRecord_MeetsLevelRequirement(u32);
s32 Inventory_CanAssignEquipment(u32);
s32 Inventory_IsEquipmentAssigned(u32);
void SceneHud_RefreshEquipmentIcons(u8 *p, u32 mode)
{
    s32 i;
    s32 markers;
    u32 offset;
    struct PauseWidget *widget;
    u8 *flagBase;
    u8 t1;
    {
        i = 0;
        if (!(i < 8))
            goto done1;
    loop1: {
        p[0x4D90 + i] |= (t1 = 255);
    }

        i++;
        if (i < 8)
            goto loop1;
    done1:;

        ;
    }
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
            ((struct PauseWidget *)(flagBase + 0x17E0))->flags &= ~1;
            Anim_LoadKeyframe((p + offset) + 0x17E0, p + 0xA4, (u16)(id + 0xF0), 0);
            if (((mode == 0) && (!ItemRecord_MeetsLevelRequirement(i))) ||
                ((mode == 1) && (!Inventory_CanAssignEquipment(i))))
                PauseMenu_SetSpritePalette(widget, 0x374);
            else
                PauseMenu_SetSpritePalette(widget, 0x373);
            if (Inventory_IsEquipmentAssigned(i)) {
                s32 marker = markers + 30;
                PauseMenu_ClearSpriteFlags(p, marker, 1);
                PauseMenu_SetSpritePosition(p, marker, widget->x, widget->y);
                p[0x4D90 + markers] = i;
                markers++;
            }
        }
        widget++;
        offset += t1 - 159;
        flagBase += 0x60;
        i++;
    } while (i <= 15);
    for (; markers <= 15; markers++)
        PauseMenu_AddSpriteFlags(p, markers + 30, 1);
}

#else
INCLUDE_ASM("asm/nonmatching", SceneHud_RefreshEquipmentIcons);
#endif
