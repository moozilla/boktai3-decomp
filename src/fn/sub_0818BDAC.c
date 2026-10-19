
#ifdef NONMATCHING
// fuzzy: 87.3%
// foundry draft (worker best 85.5%); what differs is mostly register allocation and instruction order
#define Armor_TriggerBehavior35 sub_0818BDAC
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

void sub_0818BDAC(void *unused0, void *unused1, struct ArmorActor *p)
{
    if (!(p->flags & 0x80)) {
        if ((u8)(p->type - 4) <= 1) {
            if (!(p->flags & 1) && p->accumulated <= 0) {
                p->accumulated = 300;
                ActorController_SetBehavior(p, 35);
            }
            return;
        }
        if (p->type == 11) {
            if ((p->flags & 1) && p->accumulated <= 0) {
                p->accumulated = 300;
                ActorController_SetBehavior(p, 35);
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0818BDAC);
#endif
