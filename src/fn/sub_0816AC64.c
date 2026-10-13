#include "global.h"
#ifdef NONMATCHING
// fuzzy: 91.1%
// 5 differing instruction runs; first diff at +0x52 (movs r0, #0xf0 vs movs r2, #0xf0)
void sub_08163F14(u8 *, u32, u32, u32, u32);
void sub_08165ACC(u8 *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void Anim_LoadKeyframe(void *, void *, u32, u32);
void sub_08163EA0(u8 *, void *);
void sub_0816AF5C(void);
void sub_0816AC64(u8 *s)
{
    u32 zero;
    u8 *a, *b, *initial;
    u16 *p;
    sub_08030BF8();
    sub_08033468();
    sub_08033590();
    sub_08030F20(1, 1, 28, 6);
    sub_08030C84(*(u32 *)(s + 0x58));
    sub_08030B78(1);
    sub_08163F14(s, *(u32 *)(s + 0x18), 0, 2, *(u32 *)(s + 0x2C));
    sub_08165ACC(s);
    *(u32 *)(s + 0x3BE8) &= ~1;
    p = (u16 *)(s + 0x3C00);
    zero = 0;
    *p = 0xFFA8;
    *(u16 *)(s + 0x3C02) = 0x20;
    initial = s + 0x3BE0;
    b = s + 0xC4;
    Anim_SetKeyframes(initial, b, 4, 2, zero);
    a = s + 0x3D00;
    Anim_LoadKeyframe(a, b, 0x1D, 1);
    Anim_SetKeyframes(a, b, 10, 1, zero);
    sub_08030DBC(0);
    sub_08163EA0(s, sub_0816AF5C);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0816AC64);
#endif
