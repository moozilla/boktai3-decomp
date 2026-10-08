#include "global.h"

void sub_08222C88(void);
void sub_08222C94(void);

s32 sub_08223490(u16 v) {
    s32 r;
    sub_08222C88();
    if (*(u16 *)0x03001728 & 0x4000) {
        *(u16 *)0x03001728 = v;
        r = 0;
    } else {
        r = -4;
    }
    sub_08222C94();
    return r;
}
