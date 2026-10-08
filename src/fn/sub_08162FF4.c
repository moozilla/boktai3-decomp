#include "global.h"
struct P { u8 f0[0x33C]; u16 a; u16 b; u8 f1[1]; u8 c; u8 f2[4]; u16 d; };
void sub_08162FF4(struct P *p)
{
    p->d++;
    if (p->d > 0x59)
        p->c = 8;
}
