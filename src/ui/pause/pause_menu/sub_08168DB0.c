#include "global.h"
struct P {
    u8 f0[0xA40];
    u8 *o;
};
void Player_SetWeaponMode(u8 *, s32);
void PauseMenu_ApplySelectedVariant(struct P *p)
{
    u8 *o = p->o;
    s8 i = *((s8 *)p + 0xF4A);
    s8 *q = (s8 *)p;
    q += i * 4;
    q += 0xF51;
    Player_SetWeaponMode(o, *q);
}
