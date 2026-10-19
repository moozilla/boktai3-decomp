#include "global.h"
#include "message_menu.h"
/* QuantityDigits_Init: builds a row of 5 sprites (8px apart) from the choice-menu
 * cursor resource (CB05/530D), sharing one palette pool index; low confidence. */
#define QuantityDigits_Init sub_081C2334

extern u8 *gUnk_030042E4;

struct Dimensions {
    u32 x : 16, y : 16, z : 16;
};

void sub_081C2334(u8 *p)
{
    struct MenuResource *resource = ScriptProc_Start(0xCB05, 0x530D);
    struct Dimensions dimensions;
    u8 *slot;
    u16 poolIndex;
    int i;

    *(struct MenuResource *)(p + 0x378) = *resource;
    ResourceHeader_Relocate(p + 0x378, resource);

    dimensions.x = 0x90;
    dimensions.y = 0x40;
    dimensions.z = 0;
    sub_0821983C(p + 0x398, p + 0x378, 0x28, 0x30, 1, 0, 0x3c, &dimensions);
    poolIndex = 0x3AC;
    *(u16 *)(p + 0x3D2) = poolIndex;
    *(void **)(p + 0x3E0) = gUnk_030042E4 + (poolIndex << 5);
    dimensions.x += 8;

    slot = p + 0x3F8;
    for (i = 0; i <= 3; i++) {
        sub_0821983C(slot, p + 0x378, 0x1e, 0x30, 1, 0, 0x3c, &dimensions);
        poolIndex = 0x3AC;
        *(u16 *)(slot + 0x3A) = poolIndex;
        *(void **)(slot + 0x48) = gUnk_030042E4 + (poolIndex << 5);
        dimensions.x += 8;
        slot += 0x60;
    }
}
