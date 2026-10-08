#include "global.h"

typedef struct { u32 a, b; } P;

void sub_08093100(u8 *p, P *q) {
    u8 *r = *(u8 **)(p + 0x3d0);
    *(P *)(r + 0x470) = *(P *)(p + 0x5c);
    *(P *)(r + 0x478) = *q;
    *(u16 *)(r + 0x430) = 0;
}
