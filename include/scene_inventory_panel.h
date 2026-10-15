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
    s8 catalog[0x134];
    u8 selection;
};
extern const struct SceneValueItemDefinition gUnk_08E61280[];
s32 sub_08030BF8(void);
u32 sub_08033468(void);
s32 sub_08030C84(const void *);
s32 sub_08030F20(s32, s32, s32, s32);
s32 sub_08030B78(s32);
s32 sub_08030DBC(s32);
s32 sub_0803343C(const u8 *);
s32 sub_08033478(s32, s32);
void sub_0805F480(struct SceneInputWidget *, struct SceneInputWidget *, struct SceneInputResource *,
                  const void *, s32);
void sub_0805F63C(struct SceneInputWidget *, struct SceneInputWidget *, struct SceneInputWidget *,
                  struct SceneInputResource *, s32);
#endif
