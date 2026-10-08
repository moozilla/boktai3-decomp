#include "global.h"

struct A { u8 f[0x1c]; u16 i; u16 j; u8 k; };
struct B { u8 f[4]; u8 d4[0x40]; u8 d44[0x20]; u32 t[1]; };
void sub_08221110(void *, u32, u32, u32, u32);

void sub_0800E0CC(struct A *a, struct B *b)
{
    sub_08221110(b->d44, b->t[a->i], b->t[a->j], a->k, 6);
    CpuSet(b->d44, b->d4, 0x10);
}
