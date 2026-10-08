#include "global.h"
void sub_081896D0(void *);
void sub_08188B7C(void *);
void sub_08189CD8(u8 *p) {
    if (!(*(u16 *)(p + 0xB32) & 0x200)) sub_081896D0(p);
    else sub_08188B7C(p);
}
