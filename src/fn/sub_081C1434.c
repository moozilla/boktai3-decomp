#include "global.h"
#include "message_menu.h"

#ifdef NONMATCHING
// fuzzy: 98.1%
// foundry draft (worker best 98.1%); what differs is mostly register allocation and instruction order
extern u8 gUnk_0824F999[];

struct RecordRowDimensions {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};

void sub_081C1434(u8 *p)
{
    u8 data[8];
    struct RecordRowDimensions dims;
    int i;
    u8 *obj;

    memcpy(data, gUnk_0824F999, 8);
    dims.y = 0x38;
    dims.z = 0;

    for (i = 0, obj = p + 0x90; i <= 7; i++) {
        dims.x = data[i];
        sub_0821983C(obj, p + 0x18, 0, 0x30, 0, 0, 0x3c, &dims);
        *(u32 *)(obj + 8) |= 1;
        obj += 0x60;
    }

    sub_0821983C(p + 0x390, p + 0x18, 0x3c, 0x30, 0, 0, 0x3c, NULL);

    obj = p + 0x3f0;
    sub_0821983C(obj, p + 0x38, 0x80, 0x30, 0, 0, 0x3c, NULL);
    *(u32 *)(obj + 8) |= 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081C1434);
#endif
