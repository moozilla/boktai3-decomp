#include "global.h"
void Route_Start(u8 *, u16, u16, u16);
void sub_08136C4C(u8 *p, u16 *q)
{
    if (*(s16 *)(p + 0x9c) > 0) {
        u16 a = q[1];
        u16 b = q[2];
        u16 c = q[3];
        Route_Start(p + 0x448, a, b, c);
    }
}
