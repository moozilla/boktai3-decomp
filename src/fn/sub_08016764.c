#include "global.h"
#include "engine.h"
s32 PaletteLevels_Configure(s32 m, s32 f, s32 t, u8 *c, u8 *a, u16 *i);
/* Script: -m, -f, -t (default 0 each), -c and -a four bytes each and -i four
   halfwords (all default 0), passed to PaletteLevels_Configure (registry 3535). */
s32 PaletteLevels_ConfigureFromScript(void)
{
    u8 c[4];
    u8 a[4];
    u16 iv[4];
    s32 m, f, t, k;
    u8 *pa;
    u16 *pi;
    m = Script_GetKeywordValue('m', 0);
    f = Script_GetKeywordValue('f', 0);
    t = Script_GetKeywordValue('t', 0);
    if (Script_SeekToKeyword('c')) {
        k = 0;
        pa = a;
        pi = iv;
        for (; k <= 3; k++)
            c[k] = Script_GetValue();
    } else {
        s32 zero;
        u8 *out;
        pa = a;
        pi = iv;
        zero = 0;
        out = c + 3;
        do {
            *out-- = zero;
        } while ((s32)out >= (s32)c);
    }
    if (Script_SeekToKeyword('a')) {
        for (k = 0; k <= 3; k++)
            pa[k] = Script_GetValue();
    } else {
        u8 *base = pa;
        s32 zero = 0;
        u8 *out = pa + 3;
        do {
            *out-- = zero;
        } while ((s32)out >= (s32)base);
    }
    if (Script_SeekToKeyword('i')) {
        u16 *out = pi;
        for (k = 0; k <= 3; k++)
            *out++ = Script_GetValue();
    } else {
        u16 *base = pi;
        s32 zero = 0;
        u16 *out = pi + 3;
        do {
            *out-- = zero;
        } while ((s32)out >= (s32)base);
    }
    return PaletteLevels_Configure(m, f, t, c, pa, pi);
}
