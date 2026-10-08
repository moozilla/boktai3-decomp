#include "global.h"

extern u32 *gUnk_020001A0;

void sub_08072C40(u32 *a) {
    u32 *base = gUnk_020001A0;
    if (base) {
        u32 *cur = (u32 *)base[10];
        u32 *prev = 0;
        while (cur) {
            if (cur == a) {
                if (prev == 0) base[10] = cur[3];
                else prev[3] = cur[3];
                base[7] = base[7] - 1;
                return;
            }
            prev = cur;
            cur = (u32 *)cur[3];
        }
    }
}
