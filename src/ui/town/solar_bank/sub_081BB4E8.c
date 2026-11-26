#include "global.h"
struct SpriteMenuItem {
    u32 flags;
    u8 pad4[0x5C];
};
struct SpriteMenuMarker {
    u32 flags;
    u8 pad4[0x28];
};
struct SpriteMenu {
    u8 pad0[0x60];
    struct SpriteMenuItem items[16];
};
void SpriteMenu_HideAll(struct SpriteMenu *menu)
{
    s32 i;
    struct SpriteMenuItem *item;
    struct SpriteMenuMarker *marker;
    u32 mask = 1;
    item = menu->items;
    for (i = 15; i >= 0; i--) {
        item->flags |= mask;
        item++;
    }
    mask = 1;
    marker = (void *)((u8 *)menu + 0x658);
    for (i = 1; i >= 0; i--) {
        marker->flags |= mask;
        marker++;
    }
}
