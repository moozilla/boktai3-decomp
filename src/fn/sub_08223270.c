#include "global.h"

void sub_08222D18(void);

u32 sub_08223270(void) {
    if (*(s32 *)0x0300531C == 0)
        sub_08222D18();
    else
        sub_08222D18();
    if (*(s32 *)0x03005314 == 1)
        *(s32 *)0x03005314 = 0;
    return 0;
}
