#include "global.h"

typedef struct { u32 a, b; } P;

void sub_0809CCB8(u8 *p, P *q) {
    u8 *r = *(u8 **)(p + 0x3d0);
    *(P *)(r + 0x414) = *(P *)(p + 0x5c);
    *(P *)(r + 0x41c) = *q;
    *(u16 *)(r + 0x3e0) = 0;
}
