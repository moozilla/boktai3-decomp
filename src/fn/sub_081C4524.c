#include "global.h"

struct S { u8 pad[0x64]; void (*f64)(void); s32 f68; };
void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void sub_081C4668(void);

void sub_081C4524(struct S *p)
{
    sub_082279A8(1, 6, 4, 4, 4, 0xFFFF, 0);
    p->f64 = sub_081C4668;
    p->f68 = 0;
}
