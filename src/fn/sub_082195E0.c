#include "global.h"

struct Obj {
    u8 pad[4];
    u8 f4;
    u8 pad5[0x17];
    u8 f1C;
};
void Anim_UnlinkFromList(struct Obj *, u32);

void Anim_Remove(struct Obj *o)
{
    if (o->f4 != 0)
        Anim_UnlinkFromList(o, o->f1C);
}
