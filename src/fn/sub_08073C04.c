#include "global.h"

extern u8 *gUnk_020004A8;
void sub_08219D38(u32 *);

void sub_08073C04(void) {
    u32 *n = *(u32 **)(gUnk_020004A8 + 0x280);
    while (n) {
        u32 *next = (u32 *)*n;
        sub_08219D38(n);
        n = next;
    }
}
