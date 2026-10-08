#include "global.h"

void sub_0819A9DC(u8 *, u32);

void sub_0819A9F0(u8 *p, u32 v) {
    if (*(p + 0x3B) == 0xff) {
        sub_0819A9DC(p, v);
    }
}
