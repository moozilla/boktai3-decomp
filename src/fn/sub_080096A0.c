#include "global.h"
struct Object {
    u8 pad[32];
    u32 flags;
};
struct Actor {
    u8 pad[25];
    s8 status;
    u8 pad1A[10];
    struct Object *object;
};
#define Actor_CanInteractWithController sub_080096A0
s32 Actor_CanInteractWithController(struct Actor *actor)
{
    struct Object *object = actor->object;
    u32 mask = 0x186c;
    u32 flags;
    flags = object->flags;
    if (flags & mask)
        return 0;
    flags &= 0x100000;
    if (!flags && actor->status < 0)
        return 1;
    return 0;
}
