#ifndef GUARD_SCENE_HUD_MAP_H
#define GUARD_SCENE_HUD_MAP_H
#include "background_controller.h"

/* Partial tilemap view of the shared 0x30-byte background record. */
struct SceneHudMapLayer {
    u8 unknown0[0x2C];
    u16 *map;
};
extern const u8 gUnk_0824DEB8[];
extern const u16 gUnk_0824DEC0[], gUnk_0824DED0[];
u16 *sub_0805DD18(u32, u32, u32);
void sub_0805DD38(u32, u32, s32);
void sub_0805DD68(u32, u32, s32);
void sub_0805DD98(s32, s32, u32, u32, s32);
void sub_0805DDE8(s32);
#endif
