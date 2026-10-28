#ifndef GUARD_SCENE_PANEL_OVERLAY_H
#define GUARD_SCENE_PANEL_OVERLAY_H

#include "scene_panel.h"

/* Record 3 literal retained for agbcc reload allocation. */
#define SCENE_OVERLAY_LAYER_ADDR 0x03004CC0 /* gBgRecords[3] */

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
void ScenePanel_CycleOverlayPalette(struct ScenePanel *);

#endif
