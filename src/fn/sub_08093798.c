#include "global.h"
struct S {
    u8 pad[0xDC + 0xF];
    u8 flags;
};
void Slime_SetStateFlag(struct S *p)
{
    u8 *q = (u8 *)p + 0xDC;
    u8 m = 8;
    q[0xF] = m | q[0xF];
}
