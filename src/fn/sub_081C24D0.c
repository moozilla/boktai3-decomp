#include "global.h"
#include "directional_interaction.h"

#ifdef NONMATCHING
// fuzzy: 87.2%
// foundry draft (worker best 87.2%); what differs is mostly register allocation and instruction order
struct Dimensions {
    u32 x : 16, y : 16, z : 16;
};

void sub_081C24D0(u8 *p)
{
    struct Dimensions dimensions;
    int i;

    dimensions.x = 0x80;
    dimensions.y = 0x38;
    dimensions.z = 0;

    AnimData_Load(p + 0x234, 0xA8E7);

    i = 0;
    {
        u32 c = 0x100;
        for (; i <= 6; i++) {
            u32 offset = i * 0x2c;
            Sprite_Init(offset + c + p, p + 0x234, 0x10);
            {
                u8 *slotAddr = offset + p;
                *(u16 *)(slotAddr + c + 0x10) = 0;
            }
            {
                u8 *base = p + offset;
                *(struct Dimensions *)(base + 0x11c) = dimensions;
                base = base + 0x107;
                *(u8 *)base = 1;
            }
            dimensions.x += 8;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081C24D0);
#endif
