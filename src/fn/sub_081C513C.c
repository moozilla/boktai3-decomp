#include "global.h"

struct S { u8 pad[0xd8]; s32 fd8; };
void sub_08220F70(u8 *, u8 *);
void sub_081C509C(struct S *);

void sub_081C513C(struct S *p)
{
    sub_08220F70((u8 *)p + 0x18, (u8 *)p + 0x78);
    p->fd8++;
    if (p->fd8 > 0x3b) sub_081C509C(p);
}
