#include "global.h"

void sub_0822B45C(void);

void sub_0822505C(void) {
    u32 *p;
    u32 m;
    sub_0822B45C();
    p = (u32 *)0x0300523C;
    m = ~2;
    *p = *p & m;
}
