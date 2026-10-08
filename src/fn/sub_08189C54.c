#include "global.h"
void sub_081885BC(void *);
void sub_0818911C(void *);
void sub_08189C54(u8 *p) {
    if (!(*(u16 *)(p + 0xB32) & 0x200)) sub_081885BC(p);
    else sub_0818911C(p);
}
