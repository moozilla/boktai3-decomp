#include "global.h"
struct Q { u8 pad[0x20]; u32 fl; u8 pad2[7]; u8 f; };
struct P { u8 pad; u8 k; };
struct Q *sub_08037DC0(void);
void sub_08037ECC(struct P *p)
{
    struct Q *q = sub_08037DC0();
    u32 m;
    if (q != 0) {
        switch (p->k) {
        case 1: m = 1; break;
        case 2: m = 4; break;
        case 3: m = 0x10; break;
        case 4: m = 0x40; break;
        case 5: m = 0x100; break;
        case 0:
        default: goto end;
        }
        q->fl |= m;
    end:
        q->f = 1;
    }
}
