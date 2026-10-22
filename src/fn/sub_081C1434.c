#include "global.h"
#include "message_menu.h"

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

    for (i = 0; i <= 7; i++) {
        dims.x = data[i];
        Widget_InitLinked(p + 0x90 + i * 0x60, p + 0x18, 0, 0x30, 0, 0, 0x3c, &dims);
        *(u32 *)(p + 0x98 + i * 0x60) |= 1;
    }

    Widget_InitLinked(p + 0x390, p + 0x18, 0x3c, 0x30, 0, 0, 0x3c, NULL);

    obj = p + 0x3f0;
    Widget_InitLinked(obj, p + 0x38, 0x80, 0x30, 0, 0, 0x3c, NULL);
    *(u32 *)(obj + 8) |= 1;
}
