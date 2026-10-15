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
void sub_0822D808(u32, struct SceneInputItemDescriptor *);
void *sub_0822E5F4(u32);
s32 sub_081590E4(const struct SceneInputItemDescriptor *);
s32 sub_081590FC(const void *);
void sub_0822D740(const void *, void *);
s32 sub_08159030(const void *, const void *);
void *sub_0822E288(u32);
s32 sub_0815A384(s32);
s32 sub_0815A3C4(s32, s32);
void SceneInputPanel_SplitThreeDigits(s32, s32 *);
void SceneInputPanel_GetEquippedValues(s32 *, s32 *, struct SceneInputItemDescriptor *, u32 *);
#endif
