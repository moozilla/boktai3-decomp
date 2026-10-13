#include "global.h"
struct Obj {
    u8 pad0[0x50];
    u32 value;
    u8 pad1[0x60];
    u16 a[4];
    u16 b[4];
    u16 c[4];
    u32 d[4];
};
void Collider_Init(void *, u16, u32, u32, u32, u32, u32);
void sub_0821FF58(void *, u32, u32, u32, u32, u32);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, u32, u32);
void sub_08074E38(struct Obj *p, void *q, u32 a, u32 b, u32 c, u32 i, u32 d, u32 e)
{
    u32 k = 0x2001;
    u32 x, y, v, z;
    Collider_Init(q, p->value, k, 0, 0x10, a, b);
    x = p->a[i];
    y = p->b[i];
    v = p->d[i];
    z = p->c[i];
    sub_0821FF58(q, x, y, d, v, z);
    Collider_SetPosition(q, (u8 *)p + 0x5c, 0);
    Collider_SetCallback(q, c, e);
}
