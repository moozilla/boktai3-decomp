#include "global.h"
s32 sub_08002C70(void);
void sub_08002CB0(u8 *p) {
    s32 current;
    current = p[1];
    if (current < sub_08002C70()) {
        p[1] += 4;
        return;
    }
    p[1] = sub_08002C70();
    p[3] = 2;
}
