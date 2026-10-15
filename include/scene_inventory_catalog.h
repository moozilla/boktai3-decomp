#ifndef GUARD_SCENE_INVENTORY_CATALOG_H
#define GUARD_SCENE_INVENTORY_CATALOG_H
#include "scene_input.h"
/* The builders initialize seven pages of eight entries. Other controller phases
 * overlay this region with transition and grid state. */
struct SceneInventoryScriptCatalog {
    u8 unknown0[0x1A94];
    u32 itemText, consumableText;
    u8 unknown1A9C[0x14];
    u8 page, pageCount, itemPageCount;
    s8 catalog[56];
};
u32 sub_08227E90(void);
#endif
