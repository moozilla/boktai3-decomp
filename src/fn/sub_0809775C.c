#include "global.h"

typedef struct { u32 a, b; } P;

void sub_0809775C(u8 *p, P *q) {
    u8 *r = *(u8 **)(p + 0x3d0);
    *(P *)(r + 0x5ec) = *(P *)(p + 0x5c);
    *(P *)(r + 0x5f4) = *q;
    *(u16 *)(r + 0x5b4) = 0;
}
