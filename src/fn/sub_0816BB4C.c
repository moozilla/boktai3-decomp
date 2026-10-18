#define MapMenu_ClampScrollAndUpdateArrows sub_0816BB4C
#include "global.h"
struct MapMenuArrow {
    u32 flags;
    u8 unknown4[0x5C];
};
struct MapMenu {
    u8 unknown0[0x38E8];
    struct MapMenuArrow arrows[4];
    u8 unknown3A68[0xEA4];
    s16 scrollX, scrollY;
    s16 minScrollX, minScrollY, maxScrollX, maxScrollY;
};
void sub_0816BB4C(struct MapMenu *menu)
{
    menu->arrows[0].flags &= ~1;
    menu->arrows[1].flags &= ~1;
    if (menu->scrollX <= menu->minScrollX) {
        menu->scrollX = menu->minScrollX;
        menu->arrows[0].flags |= 1;
    } else if (menu->scrollX >= menu->maxScrollX) {
        menu->scrollX = menu->maxScrollX;
        menu->arrows[1].flags |= 1;
    }
    menu->arrows[2].flags &= ~1;
    menu->arrows[3].flags &= ~1;
    if (menu->scrollY <= menu->minScrollY) {
        menu->arrows[2].flags |= 1;
        menu->scrollY = menu->minScrollY;
    } else if (menu->scrollY >= menu->maxScrollY) {
        menu->arrows[3].flags |= 1;
        menu->scrollY = menu->maxScrollY;
    }
}
