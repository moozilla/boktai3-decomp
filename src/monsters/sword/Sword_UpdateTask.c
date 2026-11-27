#include "monsters/sword/sword_task.h"
#define Sword_DispatchState Ivy_DispatchState
#include "global.h"
struct SwordOwner {
    u8 unknown0[0x1C];
    u32 flags;
};
void Sword_UpdateDamage(struct SwordTask *);
void Ivy_DispatchState(void *);
void Sword_UpdateComponents(void *);
s32 Sword_UpdateTask(struct SwordTask *p)
{
    if (((struct SwordOwner *)p->target)->flags & 4)
        return 1;
    Sword_UpdateDamage(p);
    if (p->health <= 0 || (p->attack.flags & 4)) {
        struct SwordTaskStatus *status = &p->status;
        u32 mask = 1;
        status->flags |= mask;
    } else {
        struct SwordTaskStatus *status = &p->status;
        u32 mask = ~1;
        status->flags &= mask;
    }
    Ivy_DispatchState(p);
    Sword_UpdateComponents(p);
    p->ticks++;
    return 0;
}
