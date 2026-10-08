#include "global.h"

extern const u8 gUnk_0824F5C0[];
void sub_0824923C(u8 *, u32);
void sub_08195334(u8 *);

void sub_08197260(u8 *p) {
    const u32 *tbl = (const u32 *)gUnk_0824F5C0;
    sub_0824923C(p, tbl[*(p + 0x7249)]);
    sub_08195334(p);
}
