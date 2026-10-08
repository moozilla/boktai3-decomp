#include "global.h"
void sub_08188B7C(void *);
void sub_081896D0(void *);
void sub_08189C80(u8 *p) {
    if (!(*(u16 *)(p + 0xB32) & 0x200)) sub_08188B7C(p);
    else sub_081896D0(p);
}
