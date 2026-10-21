#include "global.h"

struct ArmorActor {
    u8 pad0[0xb32];
    u16 flags;
    u8 pad1[4];
    u8 type;
    u8 pad2[0x1126 - 0xb39];
    s16 accumulated;
};
void ActorController_SetBehavior(struct ArmorActor *, s32);

void Armor_TriggerBehavior35(void *unused0, void *unused1, struct ArmorActor *p)
{
    struct ArmorActor *q = p;
    if (!(p->flags & 0x80)) {
        if ((u8)(p->type - 4) <= 1) {
            if (!(p->flags & 1) && p->accumulated <= 0) {
                p->accumulated = 300;
                ActorController_SetBehavior(p, 35);
            }
            return;
        }
        if (p->type == 11) {
            if ((p->flags & 1) && q->accumulated <= 0) {
                q->accumulated = 300;
                ActorController_SetBehavior(q, 35);
            }
        }
    }
}
