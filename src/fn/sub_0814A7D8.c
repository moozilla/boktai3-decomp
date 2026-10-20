#include "global.h"
#define Player_CalculateTotalAttackPower sub_0814A7D8

struct P0814A7D8 {
    u8 f0[0x424];
    u16 f424;
    u16 f426;
    u8 f428[0x444 - 0x428];
    u32 f444;
    u8 f448[0xB44 - 0x448];
    u8 fB44[0xC78 - 0xB44];
    void *fC78[3];
};
s32 Player_CalculateBaseAttackPower(void *);
s32 sub_08249240(void *, s32, void *);
s32 Div(s32, s32);
static inline u32 tst(u32 *a, u32 m)
{
    return *a & m;
}

s32 Player_CalculateTotalAttackPower(struct P0814A7D8 *p)
{
    s32 i;
    s32 zero;
    s32 power = Player_CalculateBaseAttackPower(p);
    zero = 0;
    for (i = 0; i < 3; i++) {
        if (p->fC78[i] != 0)
            power += sub_08249240(p->fB44, power, p->fC78[i]);
    }
    if (p != 0 && tst(&p->f444, 0x08000000) != zero)
        power += Div(p->f424 * 20, p->f426);
    return power;
}
