#include "global.h"
struct Pair {
    u32 a, b;
};
struct Elem {
    u8 pad[0x1c];
    struct Pair coords;
    u8 tail[8];
};
struct Owner {
    u8 pad[0xe8];
    struct Elem elems[2];
    u8 resource[1];
};
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectTiles(void *, u32);
void sub_08235A5C(struct Owner *p)
{
    s32 i;
    AnimData_Load(p->resource, 0x24ba);
    for (i = 0; i < 2; i++) {
        Sprite_Init(&p->elems[i], p->resource, 1);
        AnimData_SelectTiles(p->resource, 0x2e5);
        p->elems[i].coords = *(struct Pair *)(*(u8 **)((u8 *)p + 0x18) + 0x30);
    }
}
