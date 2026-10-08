#include "global.h"

void sub_08224524(void);
void sub_08223E28(void);
void sub_082455DC(void *);
void sub_082455E8(void *);

u32 sub_082235C0(u32 a, u32 b) {
    u32 r;
    if (a != 0) {
        u16 z;
        u8 *p;
        u16 *q = &z;
        *q = 0;
        p = (u8 *)0x03005390;
        CpuSet(&z, p, 0x01000024);
        p[6] = 0xff;
        *(u32 *)(p + 0x40) = a;
        *(u32 *)(p + 0x44) = b;
        sub_082455DC(sub_08224524);
        sub_082455E8(sub_08223E28);
        r = 0;
    } else {
        r = 4;
    }
    return r;
}
