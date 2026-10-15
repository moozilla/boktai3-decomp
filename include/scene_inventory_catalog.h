#ifndef GUARD_SCENE_INVENTORY_CATALOG_H
#define GUARD_SCENE_INVENTORY_CATALOG_H
#include "scene_input.h"
/* The builders clear seven pages of eight entries. The wider phase workspace
 * overlaps transition/grid storage; script population relies on ROM data limits. */
struct SceneInventoryScriptCatalog {
    u8 unknown0[0x1A8C];
    u32 equipmentText, equipmentLabel;
    u32 itemText, consumableText;
    u8 unknown1A9C[0x14];
    u8 page, pageCount, itemPageCount;
    s8 catalog[0x134];
};
u32 sub_08227E90(void);
#endif
