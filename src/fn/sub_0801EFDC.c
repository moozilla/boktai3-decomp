// MATCHING_CONCESSION: register-pinning
#include "global.h"
void sub_0822B3BC(int);
void sub_0801EF5C(void);
void SceneObject_AnimateActive(u8 *s)
{
    if ((*(u32 *)(s + 0x38) & 0xffff00) == 0) {
        sub_0822B3BC(0xce);
        *(u16 *)(*(u8 **)(s + 0x2c) + 6) &= ~3;
        asm("" ::: "memory");
        {
            u16 *flags = (u16 *)(*(u8 **)(s + 0x2c) + 6);
            int old = *flags;
            int result;
            result = 2;
            result |= old;
            *flags = result;
        }
        asm("" ::: "memory");
        (*(u8 **)(s + 0x2c))[4] = ((*(u8 **)(s + 0x2c))[4] & 0xf0) | s[0x3b];
    }
    if (++s[0x3a] > 7) {
        s[0x39]++;
        s[0x3a] = 0;
        if (s[0x39] > 1) {
            {
                int one = 1;
                s[0x37] = one;
            }
            *(void **)(s + 0x30) = sub_0801EF5C;
        }
    }
}
