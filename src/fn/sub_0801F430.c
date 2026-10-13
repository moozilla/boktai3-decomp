// MATCHING_CONCESSION: register-pinning
#include "global.h"
void sub_0822B2F8(int);
void sub_0822B358(int);
void SceneObject_UpdateRaised(void);
void SceneObject_UpdateRaising(u8 *s)
{
    *(u16 *)(s + 0x80) += 15;
    if (*(s16 *)(s + 0x80) > *(s16 *)(s + 0x88)) {
        *(u16 *)(s + 0x80) = *(u16 *)(s + 0x88);
        sub_0822B358(0x152);
        sub_0822B2F8(0x154);
        *(u16 *)(s + 0x82) = 30;
        s[0x37] = 0;
        *(u16 *)(*(u8 **)(s + 0x2c) + 6) &= ~3;
        asm("" ::: "memory");
        *(u16 *)(*(u8 **)(s + 0x2c) + 6) |= 2;
        asm("" ::: "memory");
        (*(u8 **)(s + 0x2c))[4] = ((*(u8 **)(s + 0x2c))[4] & 0xf0) | s[0x3b];
        *(u16 *)(*(u8 **)(s + 0x2c)) |= 1;
        *(void **)(s + 0x30) = SceneObject_UpdateRaised;
    }
    {
        int base = *(u16 *)(s + 0x7a);
        {
            register int offset asm("r5") = *(u16 *)(s + 0x80);
            base += offset;
        }
        *(u16 *)(s + 0x72) = base;
    }
}
