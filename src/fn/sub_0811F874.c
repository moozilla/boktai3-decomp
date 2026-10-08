#include "global.h"
struct P { void (*cb)(void); u8 p0; u8 sel; u8 pad[0xa]; u8 fl; u8 pad2[7]; u32 v; };
void sub_08219DD8(void *, u32);
s32 sub_0811E534(void *, s32);
void sub_0811F4C4(void);
void sub_0811F6CC(void);
void sub_0811F538(void);
void sub_0811F75C(void);
void sub_0811F7EC(void);
void sub_0811F4DC(void);
void sub_0811F5C4(void);
void sub_0811F874(u8 *s, u32 v)
{
    u8 *a = s + 0x48;
    struct P *p = (struct P *)(s + 0x314);
    u32 z = 0;
    void (*f)(void);
    u8 *q;
    p->cb = (void (*)(void))z;
    p->fl = z;
    p->v = v;
    q = s + 0x318;
    sub_08219DD8(q, 0xc);
    if (sub_0811E534(q, *(s8 *)(a + 0xe)) < 0) {
        p->cb = sub_0811F4C4;
        p->fl = z;
    } else {
        switch (p->sel) {
        case 0: f = sub_0811F6CC; goto set;
        case 1: f = sub_0811F538; goto set;
        case 2: f = sub_0811F75C; goto set;
        case 3: f = sub_0811F7EC; goto set;
        case 4: f = sub_0811F4DC; goto set;
        case 5: f = sub_0811F5C4;
        set:
            p->cb = f;
            p->fl = 1;
            break;
        }
    }
}
