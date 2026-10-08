#include "global.h"
extern u8 *gUnk_02000490;
void sub_0805431C(u8 *a, u32 b, u8 *c)
{
    u8 *r3 = c + 0xFC;
    if (*r3 == 0) {
        u32 m = 0x20;
        if (*(u32 *)(a + 0x38) & m) {
            *(u16 *)(c + 0xF8) += 1;
            *r3 = m;
            gUnk_02000490[0x19] = 1;
        }
    }
}
