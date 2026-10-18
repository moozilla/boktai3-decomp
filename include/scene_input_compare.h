#ifndef GUARD_SCENE_INPUT_COMPARE_H
#define GUARD_SCENE_INPUT_COMPARE_H
#include "scene_input_panel.h"
/* Descriptor copied by 0822D808; its full packed meaning is not yet recovered. */
struct SceneInputItemDescriptor {
    u32 words[8];
};
struct SceneInputEquipmentView {
    u8 unknown0[0x50];
    s16 selected;
    u8 unknown52[0x16];
    s16 primary[8], secondary[8];
};
void Inventory_GetEquippedDescriptor(u32, struct SceneInputItemDescriptor *);
void *GameState_GetUnlockDescriptor(u32);
s32 Player_GetEquipmentValue(const struct SceneInputItemDescriptor *);
s32 Player_GetEquipmentLevelBonus(const void *);
void Inventory_InitEquipmentEntryFromDescriptor(const void *, void *);
s32 sub_08159030(const void *, const void *);
void *GameState_GetConsumableDescriptor(u32);
s32 Player_GetCurrentAttribute(s32);
s32 Player_GetCandidateAttribute(s32, s32);
void SceneInputPanel_SplitThreeDigits(s32, s32 *);
void SceneInputPanel_GetEquippedValues(s32 *, s32 *, struct SceneInputItemDescriptor *, u32 *);
#endif
