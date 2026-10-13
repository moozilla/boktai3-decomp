#include "global.h"

struct S {
    u8 f[0x9c];
    u8 a;
};
u32 Sprite_Remove(void *);
u32 sub_08046A00(struct S *p)
{
    Sprite_Remove(&p->a);
    return 0;
}
