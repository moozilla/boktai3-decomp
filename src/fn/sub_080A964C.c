#include "global.h"

struct Entry080A9 {
    u8 sprite[0x128];
};
struct Owner080A9 {
    u8 pad0[0x3d4];
    struct Entry080A9 entries[4];
    u8 pad[0x8da - 0x3d4 - 4 * 0x128];
    s16 count;
};
void Sprite_Remove(void *);

void sub_080A964C(struct Owner080A9 *p)
{
    s32 i;
    for (i = 0; i < p->count; i++)
        Sprite_Remove(&p->entries[i]);
}
