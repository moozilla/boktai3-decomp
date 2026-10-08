#include "global.h"
void sub_0818911C(void *);
void sub_081885BC(void *);
void sub_08189CAC(u8 *p) {
    if (!(*(u16 *)(p + 0xB32) & 0x200)) sub_0818911C(p);
    else sub_081885BC(p);
}
