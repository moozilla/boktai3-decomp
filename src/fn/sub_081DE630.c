#include "global.h"
struct DigitPos {
    u32 a, b;
};
struct DigitWidget {
    u8 pad0[8];
    u32 flags;
    u8 pad0c[0x60 - 0xc];
};
struct DateMenu {
    u8 pad0[0x78];
    u8 resource[0x238 - 0x78];
    struct DigitWidget digits[12];
    u8 pad6b8[0x6ec - 0x6b8];
    s32 values[5];
};
extern const struct DigitPos gUnk_0824FCCC[12];
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, const void *);
void sub_081DE630(struct DateMenu *menu)
{
    struct DigitPos pos[12];
    s32 i;
    memcpy(pos, gUnk_0824FCCC, sizeof(pos));
    for (i = 0; i < 12; i++) {
        Widget_InitLinked(&menu->digits[i], menu->resource, 0, 0x30, 0, 0, 0, &pos[i]);
        menu->digits[i].flags |= 1;
    }
    menu->values[0] = 0;
    menu->values[1] = 0;
    menu->values[2] = 0;
    menu->values[3] = 0;
    menu->values[4] = 0;
}
