#include "global.h"

void CpuSet(const void *src, void *dest, u32 control);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);

void sub_0801A6D0(u8 *p)
{
    u32 zero = 0;
    u16 buf[4];
    u16 *bp = buf;
    int i;
    u8 *q = p + 0xd34;
    CpuSet(&zero, buf, 0x05000002);
    for (i = 0x12; i >= 0; i--) {
        sub_0821983C(q, p + 0x6c, 0, 0x11, 0, 0, 0x3c, bp);
        q -= 0x60;
    }
    *bp = 0x20;
    buf[1] = 0;
    sub_0821983C(p + 0x614, p + 0x4c, 0, 0x11, 0, 0, 0x3c, buf);
}
