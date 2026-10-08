#include "global.h"
void sub_08181778(void);
void sub_0818171C(void *);
void sub_081816B0(void *);
void sub_08181964(u8 *s)
{
    u32 *t;
    sub_08181778();
    switch (*(u8 *)(s + 0xb38)) {
    case 3: case 5: case 7: case 9: case 0xd: case 0xf:
        sub_0818171C(s);
        break;
    default:
        break;
    }
    t = (u32 *)(s + 0x650);
    sub_081816B0(t);
    t[0] = 0;
    t[1] = 0;
}
