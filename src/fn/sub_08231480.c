#include "global.h"

struct EffectGroup {
    u8 active;
    u8 pad01[0x12f];
    struct EffectGroup *previous;
    struct EffectGroup *next;
};
struct EffectGroupPool {
    u8 pad00[0x20];
    struct EffectGroup *head;
};
s32 EffectGroup_Unlink(struct EffectGroupPool *pool, struct EffectGroup *group)
{
    struct EffectGroup *previous = group->previous;
    struct EffectGroup *next = group->next;
    if (!group->active)
        return -1;
    if (previous)
        previous->next = next;
    else
        pool->head = next;
    if (next)
        next->previous = previous;
    return group->active = 0;
}
