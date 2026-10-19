#include "global.h"
#include "directional_interaction.h"
/* IconRow_Init: loads an animation (key 0xA8E7) and lays out 6 sprites 8px apart
 * using frame ids 3,4,7,8,9,10; low confidence on the exact widget. */

struct Dimensions {
    u32 x : 16, y : 16, z : 16;
};

void IconRow_Init(u8 *p)
{
    struct Dimensions dimensions;
    u8 *slot;
    int i;

    dimensions.x = 0x84;
    dimensions.y = 0x30;
    dimensions.z = 0;

    AnimData_Load(p + 0x358, 0xA8E7);

    i = 0;
    slot = p + 0x250;
    for (; i <= 5; i++) {
        Sprite_Init(slot, p + 0x358, 0x11);
        if (i <= 1)
            *(u16 *)(slot + 0x10) = i + 3;
        else
            *(u16 *)(slot + 0x10) = i + 5;
        *(struct Dimensions *)(slot + 0x1c) = dimensions;
        *(u8 *)(slot + 7) = 1;
        dimensions.x += 8;
        slot += 0x2c;
    }
}
