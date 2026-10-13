// MATCHING_CONCESSION: register-pinning
#include "global.h"
u8 *sub_0821A2C4(int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
void SceneObject_UpdateRaised(void);
int SceneObjects_ScriptRaise(void)
{
    register u8 *p asm("r5") = sub_0821A2C4(0x85e6);
    register int found asm("r6");
    register int i asm("r4");
    register int id asm("r8");
    if (!p)
        return -1;
    found = 0;
    if (!Script_SeekToKeyword(0x6e))
        return 0;
    id = Script_GetValue();
    i = 0;
    if (found < *(u16 *)(p + 0x1a)) {
        u8 **base = (u8 **)(p + 0x42c);
        int zero;
        register int count asm("r2");
        void *fn;
        int mask;
        asm("" : : "r"(base));
        zero = 0;
        asm("" : "+r"(zero));
        mask = 0xfffc;
        fn = SceneObject_UpdateRaised;
        do {
            u8 *s;
            s = *base + i * 140;
            if (*(u16 *)(s + 0x34) == id) {
                {
                    u16 *out = (u16 *)(s + 0x82);
                    register int clear asm("r0");
                    clear = 0;
                    *out = clear;
                }
                s[0x37] = zero;
                *(void **)(s + 0x30) = fn;
                s[0x39] = zero;
                s[0x3a] = zero;
                {
                    u8 *gfx = *(u8 **)(s + 0x2c);
                    int old = *(u16 *)(gfx + 6);
                    int result;
                    result = mask;
                    asm("" : "+r"(result));
                    result &= old;
                    *(u16 *)(gfx + 6) = result;
                }
                asm("" ::: "memory");
                *(u16 *)(*(u8 **)(s + 0x2c) + 6) |= 2;
                asm("" ::: "memory");
                (*(u8 **)(s + 0x2c))[4] = ((*(u8 **)(s + 0x2c))[4] & 0xf0) | s[0x3b];
                {
                    int amount = *(u16 *)(s + 0x88);
                    *(u16 *)(s + 0x80) = amount;
                }
                {
                    u16 *baseline = (u16 *)(s + 0x72);
                    register int amount asm("r0");
                    amount = *(u16 *)(s + 0x88);
                    asm("" : "+r"(amount));
                    *(u16 *)(s + 0x1e) = amount + *baseline;
                }
                found = 1;
            }
            i++;
            count = *(u16 *)(p + 0x1a);
        } while (i < count);
    }
    return found;
}
