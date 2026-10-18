#include "global.h"
#ifdef NONMATCHING
// fuzzy: 73.1%
// 11 differing instruction runs; size -4 bytes; first diff at +0xA (ldr r0, [pc, #0x80] vs ldr r0, [pc, #0x7c])
struct PairED {
    u32 a, b;
};
u32 Player_GetConfiguredActionPower(u8 *);
void AnimData_SelectPaletteBank(u8 *, u32);
void Collider_SetAttackProperties(u8 *, u32, u32, u32, u32, u32);
void sub_0823EDDC(u8 *p)
{
    u32 resource, zero;
    u8 *angle;
    *(struct PairED *)(p + 0xbe8) = *(struct PairED *)(p + 0x30);
    {
        u32 y = *(u16 *)(p + 0xbea) + 230;
        zero = 0;
        *(u16 *)(p + 0xbea) = y;
    }
    {
        u32 direction = ((p[0x3a4] + 5) & 7) << 5;
        angle = p + 0xbfc;
        *angle = direction;
    }
    resource = Player_GetConfiguredActionPower(p);
    AnimData_SelectPaletteBank(p + 0xb70, 76);
    Collider_SetAttackProperties(p + 0xb8c, resource, *(u16 *)(p + 0xbfc), 0x2000, zero, *(u16 *)(p + 0xbfe));
    p[0xbce] = *angle;
    *(u16 *)(p + 0xc0a) = zero;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823EDDC);
#endif
