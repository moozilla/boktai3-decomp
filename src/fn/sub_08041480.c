#include "global.h"

void sub_0803384C(s32);

void sub_08041480(u8 *p) {
    s32 *e = (s32 *)(p + 0xf8);
    s32 i = 5;
    do {
        if (*e >= 0) {
            sub_0803384C(*e);
        }
        e++;
        i--;
    } while (i >= 0);
}
