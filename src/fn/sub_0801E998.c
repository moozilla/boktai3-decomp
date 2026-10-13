// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObjects_ScriptLower sub_0801E998
u8 *sub_0821A2C4(int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
void sub_0801F5B8(void);
int SceneObjects_ScriptLower(void)
{
    register u8 *p asm("r5") = sub_0821A2C4(0x85e6);
    int found;
    int i;
    register int id asm("r8");
    if (!p)
        return -1;
    found = 0;
    if (!Script_SeekToKeyword(0x6e))
        return -1;
    id = Script_GetValue();
    asm("" : "+r"(id));
    i = 0;
    if (found < *(u16 *)(p + 0x1a)) {
        u8 **base = (u8 **)(p + 0x42c);
        int zero;
        int one;
        asm("" : : "r"(base));
        asm("" : "+r"(found));
        zero = 0;
        asm("" : "+r"(zero));
        one = 1;
        do {
            u8 *s;
            s = *base + i * 140;
            if (*(u16 *)(s + 0x34) == id) {
                *(u16 *)(s + 0x82) = zero;
                s[0x37] = one;
                {
                    register void *fn asm("r2") = sub_0801F5B8;
                    *(void **)(s + 0x30) = fn;
                }
                {
                    int pose = s[0x84];
                    s[0x39] = pose;
                }
                {
                    u8 *out = s + 0x3a;
                    register int clear asm("r1");
                    clear = 0;
                    *out = clear;
                }
                *(u16 *)(*(u8 **)(s + 0x2c) + 6) &= ~2;
                asm("" ::: "memory");
                *(u16 *)(*(u8 **)(s + 0x2c) + 6) |= 3;
                asm("" ::: "memory");
                (*(u8 **)(s + 0x2c))[4] = ((*(u8 **)(s + 0x2c))[4] & 0xf0) | (s[0x3b] + 1);
                *(u16 *)(s + 0x80) = zero;
                *(u16 *)(s + 0x1e) = *(u16 *)(s + 0x72);
                found = 1;
            }
            i++;
        } while (i < *(u16 *)(p + 0x1a));
    }
    return found;
}
