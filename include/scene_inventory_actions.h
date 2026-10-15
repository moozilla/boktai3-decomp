#ifndef GUARD_SCENE_INVENTORY_ACTIONS_H
#define GUARD_SCENE_INVENTORY_ACTIONS_H
#include "scene_input_panel.h"
#include "scene_input_grid.h"
struct SceneHudDigitView;
#include "scene_input_compare.h"
struct SceneInventoryActionView {
    struct SceneInputPanel display;
    u8 unknown14E8[0x1AA6 - 0x14E8];
    u8 valueMode;
    u8 unknown1AA7[9];
    u8 page, unknown1AB1[2];
    /* This phase-dependent view overlaps the transition pair at 0x1AF0. */
    s8 catalog[0x134];
    u8 selection, unknown1BE8[2], savedSelection;
};
struct SceneInventoryTransitionView {
    u8 unknown0[0x1AF0];
    struct SceneInputPair pair;
};
struct SceneInventoryActionFlags {
    u32 flags;
    u8 unknown4[0x5C];
};
struct SceneInventoryActionFlagsView {
    u8 unknown0[0xB0];
    struct SceneInventoryActionFlags widgets[54];
};
struct SceneInventoryActionDefinition {
    u8 unknown0[6];
    u16 baseValue;
};
s32 sub_0822D6E0(u32);
s32 sub_0805F7E0(u32);
struct SceneInventoryActionDefinition *sub_08227F5C(u32);
s32 sub_0822CDA8(u32);
s32 sub_0805F87C(u32);
u32 sub_0805F86C(s32);
s32 sub_0822E104(u32);
s32 sub_0805F8E8(u32);
s32 sub_0805F8D8(u32);
void SceneHud_ShowFourDigits(struct SceneHudDigitView *, const struct ContactPosition *, s32, u32);
void SceneHud_HideFourDigits(struct SceneHudDigitView *);
s32 sub_08060528(s32);
#endif
