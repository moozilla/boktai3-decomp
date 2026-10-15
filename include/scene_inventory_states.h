#ifndef GUARD_SCENE_INVENTORY_STATES_H
#define GUARD_SCENE_INVENTORY_STATES_H
#include "scene_inventory_control.h"
struct SceneInventoryActionView;
struct SceneInventoryConsumableSlot {
    u8 unknown0[0x160];
    s16 id;
};
void SceneInput_SaveSelection(struct SceneInputGridView *);
void sub_080614B4(struct SceneInventoryControlView *, s32);
void SceneInputPanel_ShowDescription(void *, s32, const void *, s32);
s32 sub_08061DA0(struct SceneInventoryControlView *);
s32 sub_08062190(struct SceneInventoryControlView *);
s32 sub_08062298(struct SceneInventoryControlView *);
s32 sub_080623C0(struct SceneInventoryControlView *);
s32 sub_08062414(struct SceneInventoryControlView *);
s32 sub_080624C8(struct SceneInventoryControlView *);
s32 sub_080625E0(struct SceneInventoryControlView *);
s32 sub_08062634(struct SceneInventoryControlView *);
s32 sub_08062778(struct SceneInventoryControlView *);
s32 sub_080627C4(struct SceneInventoryControlView *);
s32 sub_08062818(struct SceneInventoryControlView *);
s32 sub_08062324(struct SceneInventoryControlView *);
void sub_0806055C(struct SceneInventoryActionView *);
void sub_08060660(struct SceneInventoryActionView *);
void sub_08060708(struct SceneInventoryActionView *);
void sub_080607B0(struct SceneInventoryActionView *);
void sub_0806097C(struct SceneInventoryActionView *);
void sub_08060A70(struct SceneInventoryActionView *);
void sub_08060B94(struct SceneInventoryActionView *);
void sub_08060C54(struct SceneInventoryActionView *);
void sub_08060D0C(struct SceneInventoryActionView *);
void sub_08060DB4(struct SceneInventoryActionView *);
void sub_08060EA4(struct SceneInventoryActionView *);
void sub_08060F4C(struct SceneInventoryActionView *);
#endif
