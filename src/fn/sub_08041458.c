#include "global.h"

void sub_08033778(s32);

void sub_08041458(u8 *p) {
    s32 m = -1;
    s32 *e = (s32 *)(p + 0xf0);
    s32 i = 7;
    do {
        if (*e >= 0) {
            sub_08033778(*e);
            *e = m;
        }
        e++;
        i--;
    } while (i >= 0);
}
