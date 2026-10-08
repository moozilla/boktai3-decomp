#include "global.h"

extern const u8 gUnk_0824F67C[];
void sub_0824923C(u8 *, u32);
void sub_0821FE40(u8 *);
void sub_0821FF24(u8 *, u8 *, u32);

void sub_08198B08(u8 *p) {
    const u32 *tbl = (const u32 *)gUnk_0824F67C;
    sub_0824923C(p, tbl[*(p + 0x157)]);
    sub_0821FE40(p + 0xF4);
    sub_0821FF24(p + 0xF4, p + 0x48, 0);
}
