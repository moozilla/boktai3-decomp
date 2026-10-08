#include "global.h"
struct S { u8 f[0x48]; u8 pad[5]; u8 b; };
struct T { u8 f[8]; u32 v; };
void sub_08219DD8(void *, u32);
void sub_0811E5D0(void *, u32);
void sub_0811FA90(u8 *s, u32 v)
{
    u8 *a = s + 0x48;
    struct T *t = (struct T *)(s + 0x518);
    t->v = v;
    sub_08219DD8(t, 8);
    sub_0811E5D0(t, a[5]);
}
