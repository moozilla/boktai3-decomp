#include "global.h"
typedef struct {
    u32 id;
    u8 active, pad[171];
} Slot;
typedef struct {
    u8 pad[24];
    Slot slots[16];
} Manager;
Slot *SpritePool_Find(Manager *p, u32 id)
{
    Slot *q;
    int i;
    if (!id)
        return 0;
    q = p->slots;
    for (i = 0; i < 16; i++, q++)
        if (q->active && q->id == id)
            return q;
    return 0;
}
