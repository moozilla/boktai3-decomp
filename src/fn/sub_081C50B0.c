#include "global.h"

struct S {
    u8 pad[0xd4];
    void (*fd4)(void);
    u32 fd8;
};
void Anim_SetKeyframes(u8 *, u8 *, u32, u32, u32);
void sub_081C513C(void);

void sub_081C50B0(struct S *p)
{
    void (*f)(void);
    Anim_SetKeyframes((u8 *)p + 0x18, (u8 *)p + 0x78, 0x3b, 2, 0);
    f = sub_081C513C;
    p->fd4 = f;
    p->fd8 = 0;
}
