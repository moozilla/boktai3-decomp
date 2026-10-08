#include "global.h"

extern u32 *gUnk_020004AC;
void sub_0824923C(u32, u32);
void sub_08219D38(u32);

void sub_08071434(void) {
    u32 *n = gUnk_020004AC;
    u8 *e;
    if (n) {
        while ((e = (u8 *)n[1]) != 0) {
            u32 a, b;
            n = (u32 *)*n;
            a = *(u32 *)(e + 0x3d0);
            b = *(u32 *)(e + 0x25c);
            sub_0824923C(a, b);
            sub_08219D38(a);
        }
    }
}
