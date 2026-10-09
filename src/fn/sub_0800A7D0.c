#include "global.h"
extern u8 *gUnk_0200003C;
void sub_08009F34(void *, void *);
void sub_0800A7D0(void) {
    u8 *base = gUnk_0200003C;
    s32 i;
    u8 *item;
    if (base == 0)
        return;
    item = base + 0x2C;
    for (i = 0xF; i >= 0; i--) {
        if (item[5] != 0)
            sub_08009F34(base, item);
        item += 0xA0;
    }
}
