#include "global.h"

struct E {
    u8 f[4];
    u8 a;
    u8 g[0x27];
};
void Sprite_Remove(void *);

void sub_081ADD7C(struct E *e)
{
    s32 i;
    for (i = 5; i >= 0; i--) {
        if (e->a != 0)
            Sprite_Remove(e);
        e++;
    }
}
