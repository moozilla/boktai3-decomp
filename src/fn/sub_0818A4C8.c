#include "global.h"

extern const u8 gUnk_0824F0CC[];
void sub_0824923C(u8 *, u32);

void sub_0818A4C8(u8 *p) {
    if ((*(u16 *)(p + 0xB32) & 0x80) == 0) {
        const u32 *tbl = (const u32 *)gUnk_0824F0CC;
        sub_0824923C(p, tbl[*(p + 0xB38)]);
    }
}
