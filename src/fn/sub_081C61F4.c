#include "global.h"
struct E {
    u8 pad[0x60];
};
struct S {
    u8 f[0x118];
    struct E e[6];
};
void Anim_Remove(void *);
void sub_081C61F4(struct S *p)
{
    s32 i;
    struct E *e = p->e;
    for (i = 5; i >= 0; i--) {
        Anim_Remove(e);
        e++;
    }
    Anim_Remove((u8 *)p + 0x58);
    Anim_Remove((u8 *)p + 0xb8);
}
