#include "global.h"
struct P {
    u8 f0[0xA40];
    u8 *o;
};
void Player_SetActionRequest(u8 *, s32);
void PauseMenu_ApplySelectedEquipment(struct P *p)
{
    s8 *q = (s8 *)p + 0xC72;
    s8 *r;
    u8 *o;
    if (*q >= 0 && *((u8 *)p + 0xC70) != 0) {
        o = p->o;
        r = (s8 *)p;
        r += *q * 4;
        r += 0xC79;
        Player_SetActionRequest(o, *r);
    } else {
        Player_SetActionRequest(p->o, -1);
    }
}
