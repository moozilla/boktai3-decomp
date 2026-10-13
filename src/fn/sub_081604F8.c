#include "global.h"

struct R {
    u32 a, b;
};
struct Q {
    u8 f00[0x20];
    struct R r;
};

void AnimData_Load(void *, s32);
void Sprite_Init(void *, void *, s32);

s32 sub_081604F8(u8 *p, struct Q *q, u32 a, u32 b)
{
    u8 *s18 = p + 0x18;
    u8 *s34 = p + 0x34;
    u32 z;
    AnimData_Load(s18, 0xB250);
    Sprite_Init(s34, s18, 1);
    z = 0;
    *(u16 *)(s34 + 0x10) = z;
    *(struct R *)(s34 + 0x1c) = q->r;
    *(u32 *)(p + 0x68) = z;
    *(u32 *)(p + 0x64) = z;
    *(struct Q **)(p + 0x74) = q;
    *(u32 *)(p + 0x6c) = a;
    *(u32 *)(p + 0x70) = b;
    return 0;
}
