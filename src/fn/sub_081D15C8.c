#include "global.h"

struct E {
    u8 pad[0x30];
};
struct S {
    u8 pad[0x7c];
    struct E e[8];
};
void Sprite_Remove(u8 *);
void Effect_Remove(struct E *);

s32 sub_081D15C8(struct S *p)
{
    struct E *e;
    s32 i;
    Sprite_Remove((u8 *)p + 0x18);
    e = p->e;
    for (i = 7; i >= 0; i--) {
        Effect_Remove(e);
        e++;
    }
    return 0;
}
