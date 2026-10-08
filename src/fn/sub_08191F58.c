#include "global.h"
void sub_08220D78(void *, void *, u32, u32, u32);
void sub_08220F70(void *, void *);
struct E { u8 pad[8]; u32 flags; u8 pad2[0x54]; };
struct S {
    u8 f0[0x541C];
    u8 x541C[0x20];
    struct E e[8];
    u8 pad[0x71DC - 0x543C - 0x60 * 8];
    u8 a[0x34];
    u8 b[8];
};
void sub_08191F58(struct S *p, u32 i) {
    if (p->a[i] == 0) {
        p->e[i].flags &= ~1;
        sub_08220D78(&p->e[i], p->x541C, 45, 1, 4);
        p->a[i]++;
        p->b[i] = 1;
    }
    sub_08220F70(&p->e[i], p->x541C);
}
