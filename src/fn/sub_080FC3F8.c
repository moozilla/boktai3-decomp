#include "global.h"

struct S080FC3F8 {
    u8 filler[0x2d4];
    u16 h;
    u8 filler2[0x3d0 - 0x2d6];
    u8 *q;
};
void Camera_RequestShake(u8);

void Axe_UpdateScreenShake(struct S080FC3F8 *s)
{
    u8 *q = s->q;
    u32 m = 0x40;
    if (!(s->h & m)) {
        u8 *p = q + 0xeb1;
        if (*p) {
            Camera_RequestShake(q[0xeb2]);
            *p = *p - 1;
        }
    }
}
