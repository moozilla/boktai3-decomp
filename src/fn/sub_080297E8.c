#include "global.h"

struct S {
    u8 f[0x888];
    u8 a;
};
void Sprite_Remove(void *);
void sub_080297E8(struct S *p)
{
    Sprite_Remove(&p->a);
}
