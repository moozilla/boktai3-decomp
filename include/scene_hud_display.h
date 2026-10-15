#ifndef GUARD_SCENE_HUD_DISPLAY_H
#define GUARD_SCENE_HUD_DISPLAY_H
#include "scene_hud_map.h"
#include "sun_clock.h"
#include "display_animation.h"
#include "scene_input.h"
#include "contact_actor.h"
struct SceneHudDigitView {
    u8 unknown0[0x20];
    u8 resource[0x12E8];
    struct SceneInputWidget label, digits[4];
};
struct SceneHudNumericView {
    u8 unknown0[0x50C];
    s32 value50C;
};
u32 sub_08228D88(void);
u32 sub_08228D94(void);
u32 sub_0805E7D0(const void *, u32);
void sub_0805E000(struct SceneInputWidget *, u32);
#endif
