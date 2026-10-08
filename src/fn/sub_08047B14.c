#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
s32 sub_08047A30(void *, u32, u32);
void sub_0821A0C0(void *);
void sub_080479F4(void);
void sub_08047A2C(void);

void *sub_08047B14(u32 a, u32 b) {
    void *p = sub_08219FBC(9, 0x48);
    if (p) {
        sub_0821A04C(p, sub_080479F4, sub_08047A2C);
        if (sub_08047A30(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
