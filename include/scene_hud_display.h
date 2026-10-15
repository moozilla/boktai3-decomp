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
u32 SunClock_GetBaseHour(void);
u32 SunClock_GetBaseMinute(void);
u32 SceneInput_TestGridFlags(const void *, u32);
void SceneHud_SetKeyframePaletteBank(struct SceneInputWidget *, u32);
#endif
