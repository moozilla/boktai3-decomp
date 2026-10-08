#include "global.h"
void sub_0822B2F8(u32);
void sub_08165C08(u8 *, u8 *, u32);
void sub_08165B7C(u8 *, u32, u32);
void sub_08163EB8(u8 *, void (*)(void), u32);
void sub_0817026C(void);
void sub_08170208(u8 *p)
{
    u16 *c = (u16 *)(p + 0xA4E);
    if (*c == 0)
        sub_0822B2F8(0x38A);
    *c = *c + 1;
    if (*c <= 0xb) {
        sub_08165C08(p + 0x2AC0, p + 0x442C, 0xc - *c);
    } else {
        sub_08165B7C(p, 0x3c, 1);
        sub_08163EB8(p, sub_0817026C, 1);
    }
}
