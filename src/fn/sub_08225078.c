#include "global.h"

void sub_0822B410(void);

void sub_08225078(void) {
    u32 m = 1;
    if (((*(u32 *)0x030053F4 | *(u32 *)0x030053F0) & m) == 0) {
        u32 *p;
        u32 f;
        sub_0822B410();
        f = 2;
        p = (u32 *)0x0300523C;
        *p = *p | f;
    }
}
