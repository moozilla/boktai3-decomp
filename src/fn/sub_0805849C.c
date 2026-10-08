#include "global.h"
struct S { u8 f[0x22e]; u16 c; };
void sub_08058130(void *, u32, u32);
void sub_08058150(void *, void (*)(void));
void sub_080584D4(void);
void sub_0805849C(struct S *p) {
    p->c++;
    if (p->c > 7) {
        sub_08058130(p, 2, 13);
        sub_08058150(p, sub_080584D4);
    }
}
