#include "global.h"
#include "scene_input_compare.h"
#include "scene_hud_display.h"
#include "player.h"

void Hud_DrawHealthMeterCells(void *, u32, u32, u32, u32, u32, u32);
void sub_08164480(void *, s32, s32, s32, s32, s32, s32);
void InventoryPanel_UpdateSelectedUnlock(void *, s32, s32, s32, s32);
void InventoryPanel_UpdateSelectedEquipment(void *, s32, s32, s32, s32);
void InventoryGrid_UpdateStatPreview(struct SceneHudState *p)
{
    struct SceneInputItemDescriptor descriptor;
    s32 t[2];
    s32 value;
    value = p->comparisonValues[0] + p->comparisonOffsets[0] + p->comparisonDeltas[0];
    if (value > 120)
        value = 120;
    t[0] = value * 10;
    Hud_DrawHealthMeterCells(p, 1, 8, 8, 1, p->actionOwner->currentStatA, t[0]);
    value = p->comparisonValues[1] + p->comparisonOffsets[1] + p->comparisonDeltas[1];
    if (value > 120)
        value = 120;
    t[0] = value * 5 + 100;
    sub_08164480(p, 1, 8, 10, p->actionOwner->currentStatB, t[0], 1);
    Inventory_GetEquippedDescriptor((s16)gGameState->unk50, &descriptor);
    t[1] = 7;
    InventoryPanel_UpdateSelectedUnlock(p, 20, t[1], 0, p->comparisonOffsets[2]);
    InventoryPanel_UpdateSelectedEquipment(p, 26, 7, 0, p->comparisonOffsets[2]);
}
