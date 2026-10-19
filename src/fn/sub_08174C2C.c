
#ifdef NONMATCHING
// fuzzy: 82.2%
// foundry draft (worker best 81.5%); what differs is mostly register allocation and instruction order
#define InventoryGrid_UpdateStatPreview sub_08174C2C
#include "global.h"
#include "scene_input_compare.h"
#include "scene_hud_display.h"
#include "player.h"
void sub_081642CC(void *, u32, u32, u32, u32, u32, u32);
void sub_08164480(void *, s32, s32, s32, s32, s32, s32);
void InventoryPanel_UpdateSelectedUnlock(void *, s32, s32, s32, s32);
void InventoryPanel_UpdateSelectedEquipment(void *, s32, s32, s32, s32);
void sub_08174C2C(struct SceneHudState *p)
{
    struct SceneInputItemDescriptor descriptor;
    s32 value;
    s32 scaled;
    value = p->comparisonValues[0] + p->comparisonOffsets[0] + p->comparisonDeltas[0];
    if (value > 120)
        value = 120;
    scaled = value * 10;
    sub_081642CC(p, 1, 8, 8, 1, p->actionOwner->currentStatA, scaled);
    value = p->comparisonValues[1] + p->comparisonOffsets[1] + p->comparisonDeltas[1];
    if (value > 120)
        value = 120;
    scaled = value * 5 + 100;
    sub_08164480(p, 1, 8, 10, p->actionOwner->currentStatB, scaled, 1);
    Inventory_GetEquippedDescriptor((s16)gUnk_02000710->unk50, &descriptor);
    InventoryPanel_UpdateSelectedUnlock(p, 20, 7, 0, p->comparisonOffsets[2]);
    InventoryPanel_UpdateSelectedEquipment(p, 26, 7, 0, p->comparisonOffsets[2]);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08174C2C);
#endif
