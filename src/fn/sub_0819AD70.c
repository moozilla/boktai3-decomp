#include "global.h"

void sub_081805C8(u8 *);
void sub_08180654(u8 *);
void sub_0819AB64(u8 *, u32);

u32 sub_0819AD70(u8 *p) {
    u8 *q = p + 0x18;
    sub_081805C8(q);
    sub_08180654(q);
    sub_0819AB64(p, 0);
    return 0;
}
