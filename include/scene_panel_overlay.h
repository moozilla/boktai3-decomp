#ifndef GUARD_SCENE_PANEL_OVERLAY_H
#define GUARD_SCENE_PANEL_OVERLAY_H

#include "scene_panel.h"

/* Literal RAM view retained for agbcc reload allocation. */
#define SCENE_OVERLAY_LAYER_ADDR 0x03004CC0 /* gUnk_03004CC0 */

struct SceneOverlayLayer {
    u8 unknown0[0x10];
    s16 tileX, tileY;
    u8 unknown14[4];
    s16 width, height;
    u8 unknown1C[4];
    s16 screenX, screenY;
    u16 *tiles, *map, *output;
};
extern struct SceneOverlayLayer gUnk_03004CC0;
struct SceneOverlayMapEntry {
    u32 unknown0;
    u16 id, unknown6;
    u16 *blocks;
};
struct SceneOverlayResource {
    u32 words[3];
    struct SceneOverlayMapEntry *maps;
    u32 tiles, extra;
};
void sub_08058B88(struct ScenePanel *);

#endif
