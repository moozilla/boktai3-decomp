#include "global.h"

void rfu_REQ_disconnect(u32);
void rfu_waitREQComplete(void);

void sub_082249A8(u8 a)
{
    u8 *p = (u8 *)0x03005390;
    u8 old = p[0xe];
    p[0xe] = 1;
    rfu_REQ_disconnect(a);
    rfu_waitREQComplete();
    p[0xe] = old;
}
