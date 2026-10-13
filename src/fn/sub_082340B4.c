#include "global.h"

struct E {
    u8 pad[0x2C];
};
struct S {
    u8 pad[0x6C];
    struct E e[8];
};
void Sprite_Remove(u8 *);
void Effect_Remove(struct E *);

s32 sub_082340B4(struct S *p)
{
    struct E *e;
    s32 i;
    Sprite_Remove((u8 *)p + 0x1C);
    e = p->e;
    for (i = 7; i >= 0; i--) {
        Effect_Remove(e);
        e++;
    }
    return 0;
}
