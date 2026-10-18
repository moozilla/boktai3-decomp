#include "global.h"

struct P0814A76C {
    u8 f0[0x336];
    s16 f336;
    u8 f338[0x420 - 0x338];
    u16 f420;
    u8 f422[0xBF4 - 0x422];
    u8 fBF4;
    u8 fBF5;
};
s32 sub_0814A530(void *, void *, s32);
s32 Player_GetSpecialAttackPower(void *, s32, s32);

s32 Player_CalculateBaseAttackPower(struct P0814A76C *p, s32 d)
{
    u8 *q = (u8 *)p + 0x32C;
    s32 sum = *(s16 *)(q + 10) + p->f420 + d;
    switch (p->fBF5) {
        case 0:
        case 1:
        case 2:
            return sub_0814A530(p, (u8 *)p + 0xB44, sum);
        case 4:
            return Player_GetSpecialAttackPower(p, p->fBF4, sum);
        default:
            return 1;
    }
}
