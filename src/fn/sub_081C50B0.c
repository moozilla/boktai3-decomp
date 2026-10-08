#include "global.h"

struct S { u8 pad[0xd4]; void (*fd4)(void); u32 fd8; };
void sub_08220D78(u8 *, u8 *, u32, u32, u32);
void sub_081C513C(void);

void sub_081C50B0(struct S *p)
{
    void (*f)(void);
    sub_08220D78((u8 *)p + 0x18, (u8 *)p + 0x78, 0x3b, 2, 0);
    f = sub_081C513C;
    p->fd4 = f;
    p->fd8 = 0;
}
