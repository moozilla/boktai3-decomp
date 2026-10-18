#include "global.h"

struct Obj {
    u8 pad[4];
    u8 f4;
    u8 pad5[5];
    u8 fA;
};

void Sprite_UnlinkFromList(struct Obj *, u32);

void Sprite_Remove(struct Obj *o)
{
    if (o->f4 != 0)
        Sprite_UnlinkFromList(o, o->fA);
}
