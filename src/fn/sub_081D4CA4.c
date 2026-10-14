#include "global.h"

struct E {
    u32 a;
    u8 pad[0x90];
};
struct S {
    u8 pad[0x28];
    struct E e[6];
};
void DisplayAnimation_Destroy(struct E *);
void Sprite_Remove(u8 *);

s32 sub_081D4CA4(struct S *p)
{
    s32 i;
    for (i = 0; i <= 5; i++) {
        if (p->e[i].a != 0)
            DisplayAnimation_Destroy(&p->e[i]);
    }
    Sprite_Remove((u8 *)p + 0x9a4);
    return 0;
}
