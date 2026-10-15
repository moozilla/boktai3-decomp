#ifndef GUARD_SCENE_INPUT_PANEL_H
#define GUARD_SCENE_INPUT_PANEL_H
/* Tentative display owner for the nine-column scene-input grid. */
#include "scene_input.h"
struct SceneInputPanel {
    u8 unknown0[0x18];
    void *backgroundResource;
    const u8 *labelResource;
};
extern u8 gUnk_03005160[];
void sub_0805EA58(struct SceneInputPanel *, s32, u32);
#endif
