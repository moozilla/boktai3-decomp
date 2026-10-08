#include "global.h"
u32 sub_080351EC(u8 *p) {
    u32 v = (u32)p;
    u8 *q = (u8 *)v + 0x32;
    v = *q;
    if (v) *q = 0;
}
