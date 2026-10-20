#include "global.h"

struct CopyWidget {
    u16 f0;
    u8 pad2[4];
    u16 f6;
    u32 f8;
    u16 fC, fE, f10, f12, f14, f16, f18;
    u8 f1A, f1B;
    u8 pad1C;
    u8 f1D;
    u8 pad1E[2];
    u32 f20, f24;
    u16 f28, f2A;
    u8 pad2C[8];
    u8 f34, f35;
    u16 f36, f38, f3A;
    u8 pad3C[0x10];
    u32 f4C, f50, f54;
};

void Widget_CopyMasked(struct CopyWidget *dst, struct CopyWidget *src, u32 mask)
{
    dst->f0 = src->f0;
    if (mask & 1)
        dst->f8 = src->f8;
    dst->fC = src->fC;
    dst->fE = src->fE;
    dst->f10 = src->f10;
    dst->f12 = src->f12;
    dst->f6 = src->f6;
    dst->f14 = src->f14;
    dst->f16 = src->f16;
    dst->f18 = src->f18;
    if (mask & 2)
        dst->f1A = src->f1A;
    if (mask & 4)
        dst->f1B = src->f1B;
    dst->f1D = src->f1D;
    if (mask & 8) {
        u32 a = src->f20;
        u32 b = src->f24;
        dst->f20 = a;
        dst->f24 = b;
    }
    dst->f28 = src->f28;
    dst->f2A = src->f2A;
    dst->f34 = src->f34;
    dst->f35 = src->f35;
    dst->f36 = src->f36;
    dst->f38 = src->f38;
    if (mask & 0x10)
        dst->f3A = src->f3A;
    dst->f4C = src->f4C;
    dst->f50 = src->f50;
    dst->f54 = src->f54;
}
