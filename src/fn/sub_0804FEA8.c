#include "global.h"
struct P { u8 f[0x10]; u16 st; u8 g[0xd2]; u8 k; };
void sub_0804F7D0(struct P *, u32, void (*)(void));
void sub_0804FB48(void);
void sub_0804F970(void);
void sub_0804F888(void);
void sub_0804FEA8(struct P *p)
{
    u32 k = p->k;
    if (k == 0) {
        p->st = k;
        sub_0804F7D0(p, 0, sub_0804FB48);
    } else if (k == 2) {
        p->st = 8;
        sub_0804F7D0(p, 2, sub_0804F970);
    } else if (k == 1) {
        p->st = 5;
        sub_0804F7D0(p, 1, sub_0804F888);
    }
}
