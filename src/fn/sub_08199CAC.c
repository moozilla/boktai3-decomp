#include "global.h"

extern const u8 gUnk_0824F6C8[];
void sub_0824923C(u8 *, u32);
void sub_0821FF24(u8 *, u8 *, u32);

void sub_08199CAC(u8 *p) {
    const u32 *tbl = (const u32 *)gUnk_0824F6C8;
    sub_0824923C(p, tbl[*(p + 0xD3)]);
    sub_0821FF24(p + 0x50, p + 0x40, 0);
}
