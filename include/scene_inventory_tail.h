#ifndef GUARD_SCENE_INVENTORY_TAIL_H
#define GUARD_SCENE_INVENTORY_TAIL_H
#include "scene_input_navigation.h"
#include "scene_input_panel.h"
/* Phase-specific controller view; the catalog/transition workspace overlaps it. */
struct SceneInventoryTail {
    struct SceneInputPanel display;
    u8 unknown14E8[0x1AA4 - 0x14E8];
    u8 interactionFlags, unknown1AA5, mode, unknown1AA7, category;
    u8 unknown1AA9, choiceState, unknown1AAB[4], choiceRepeat;
    u8 page, unknown1AB1[0x1BE4 - 0x1AB1];
    struct SceneInputGridView grid;
    u8 unknown1C14[0x1D00 - 0x1C14];
    u8 highlight;
};
void SceneInputPanel_HighlightCompare(void *, void *);
void SceneInputPanel_HighlightPrompt(void *, void *);
u32 SceneInput_NavigateGrid(struct SceneInputGridView *);
s32 sub_08061C3C(struct SceneInventoryTail *);
s32 sub_08061CFC(struct SceneInventoryTail *);
void sub_080613BC(struct SceneInventoryTail *, s32);
void sub_08061540(struct SceneInventoryTail *);
void sub_08061F18(struct SceneInventoryTail *);
void sub_08061FAC(struct SceneInventoryTail *);
void sub_08062040(struct SceneInventoryTail *);
void sub_08061B94(struct SceneInventoryTail *);
void sub_08061B48(struct SceneInventoryTail *);
s32 sub_08060864(struct SceneInventoryTail *);
s32 sub_08061000(struct SceneInventoryTail *);
s32 sub_08061080(struct SceneInventoryTail *);
void sub_08061A98(struct SceneInventoryTail *);
void sub_0806167C(struct SceneInventoryTail *);
void sub_08061780(struct SceneInventoryTail *);
void sub_08061BCC(struct SceneInventoryTail *);
#endif
