#ifndef GUARD_PRESENTATION_H
#define GUARD_PRESENTATION_H
#include "scene_hud_display.h"
#include "scene_input_compare.h"
#include "scene_value_equipment.h"
#include "player.h"
struct TextLabelStyle {
    u8 index, unknown1[0xb], specialC, unknownD[3], special10, unknown11[3], suffixKind;
};
struct PresentationEquipmentDefinition {
    u8 stringId, unknown1, requiredLevel, numberB, unknown4[2];
    u16 group;
    u8 unknown8[4], stringB, unknownD[3], stringC, unknown11[15];
};
extern const struct PresentationEquipmentDefinition gUnk_08E6092C[];
struct InventoryGridPosition {
    s16 x, y;
};
void TextLabel_FormatSuffix(struct TextLabelStyle *, u8 *);
void TextLabel_DisplaySuffix(s32, s32, struct TextLabelStyle *, u8 *);
void Presentation_ApplyGraphicData(struct TextLabelStyle *, u8 *);
void Presentation_ApplyIndexedGraphicData(s32, struct TextLabelStyle *, u8 *);
void Presentation_ApplyGraphicSettings(struct SceneValueEquipment *, const void *);
void Presentation_ApplyPackedGraphicData(const void *, const void *);
u32 Presentation_SelectGraphicResource(struct SceneValueEquipment *);
void SceneHud_RefreshEquipmentPreview(struct SceneHudState *);
void SceneHud_RefreshEquipmentValues(struct SceneHudState *);
void InventoryGrid_GetSlotPosition(void *, s32);
void SceneHud_RefreshEquipmentAvailability(struct SceneHudState *);
void SceneHud_ConfigureEquipmentTransfer(struct SceneHudState *, s32, s32, s32);
u32 SceneHud_GetEquipmentActionFlags(void *, s32);
void Presentation_DrawEntryCount(struct SceneHudState *);
void Presentation_SetAmountGlyphRows(struct SceneHudState *);
void InventoryGrid_UpdateStatPreview(struct SceneHudState *);
void InventoryGrid_LayoutStatMarkers(struct SceneHudState *);
void Presentation_UpdateListCursor(struct SceneHudState *);
void Presentation_CommitListValues(struct SceneHudState *);
void Presentation_InitListValues(struct SceneHudState *);
u32 Inventory_GetEquipmentId(u32);
struct SceneValueEquipment *Inventory_GetEquipmentEntry(u32);
void Inventory_GetEquipmentDescriptor(u32, struct SceneInputItemDescriptor *);
#endif
