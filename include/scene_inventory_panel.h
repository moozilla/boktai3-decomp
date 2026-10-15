#ifndef GUARD_SCENE_INVENTORY_PANEL_H
#define GUARD_SCENE_INVENTORY_PANEL_H
#include "scene_input_panel.h"
#include "scene_item_values.h"
struct SceneInventoryPanel {
    struct SceneInputPanel display;
    struct SceneInputWidget comparison[5][3];
    u32 unknown1A88;
    const void *equipmentText;
    const void *unknown1A90;
    const void *itemText;
    const void *consumableText;
    u8 descriptionState[0x14];
    u8 page;
    u8 unknown1AB1[2];
    /* Phase workspace: overlaps the transition pair at 0x1AF0..0x1BE4. */
    s8 catalog[0x134];
    u8 selection;
};
extern const struct SceneValueItemDefinition gUnk_08E61280[];
extern const struct SceneValueConsumableDefinition gUnk_08E60F80[];
s32 sub_08030BF8(void);
u32 sub_08033468(void);
s32 sub_08030C84(const void *);
s32 sub_08030F20(s32, s32, s32, s32);
s32 sub_08030B78(s32);
s32 sub_08030DBC(s32);
s32 sub_0803343C(const u8 *);
s32 sub_08033478(s32, s32);
void SceneInputPanel_ShowItemComparison(struct SceneInputWidget *, struct SceneInputWidget *,
                                        struct SceneInputResource *, const void *, s32);
void SceneInputPanel_ShowAttributeComparison(struct SceneInputWidget *, struct SceneInputWidget *,
                                             struct SceneInputWidget *, struct SceneInputResource *,
                                             s32);
struct SceneInventoryWidgetFlags {
    u32 flags;
    u8 unknown4[0x5C];
};
struct SceneInventoryFlagsView {
    u8 unknown0[0xB0];
    struct SceneInventoryWidgetFlags widgets[54];
};
s32 sub_0822D6E0(u32);
s32 sub_0822DFDC(u32);
u32 sub_0822D604(s32);
s32 sub_0822E224(u32);
void SceneHud_SetKeyframePaletteBank(struct SceneInputWidget *, u32);
#endif
