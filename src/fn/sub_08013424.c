#include "global.h"

void sub_0824923C(u8 *p);
void sub_08013458(void);

void sub_08013424(u8 *p) {
    *(u32 *)(p + 0xC) &= ~1;
    *(u32 *)(p + 0x34) = (u32)sub_08013458;
    sub_0824923C(p);
}
