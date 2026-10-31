#include "global.h"
void Bee_SetMovementEndpoints(u8 *, s16 *);
void Bee_AdvanceMovementCurve(u8 *);
u32 Math_Atan2(s32, s32);
void CpuSet(const void *, void *, u32);
static inline void Fill(void *dst, u32 *tmp, u32 v)
{
    *tmp = v;
    CpuSet(tmp, dst, 0x05000002);
}

void Bee_BeginMoveToPosition(u8 *s, s16 *v)
{
    u8 *p = *(u8 **)(s + 0x3d0);
    u32 stack[3];
    s16 *a;
    s16 *b;
    Bee_SetMovementEndpoints(s, v);
    Bee_AdvanceMovementCurve(s);
    {
        u32 one = 1;
        u8 *q = s + 0x2a8;
        u32 zero = 0;
        *q = zero;
        {
            s32 off = 0x2a9;
            s[off] = zero;
            s[0x2aa] = one;
            *(u32 *)(s + off + 3) = zero;
        }
        s[0x2b1] = one;
        b = (s16 *)(stack + 1);
        a = (s16 *)(s + 0x5c);
        b[0] = v[0] - a[0];
        b[1] = v[1] - a[1];
        b[2] = v[2] - a[2];
        s[0x3cd] = Math_Atan2(b[0], b[2]);
        Fill(p + 0x480, stack, zero);
    }
}
