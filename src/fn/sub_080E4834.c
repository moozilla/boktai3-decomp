#define Dog_ResumeRoute sub_080E4834
#include "scene_actor_chase.h"
// confidence=low: resumes group chase or the saved route after arrival.
#ifdef NONMATCHING
// fuzzy: 84.7%
// Remaining differences: helper expansion, register allocation and literal placement.
void sub_080E04D0(void);
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void Dog_ResumeRoute(struct SceneActorRegistryResetView *a)
{
    u8 *context = a->context;
    u8 *group = (u8 *)&a->group;
    if (Chase_TakeArrived(a))
        Chase_SetCtl(a, sub_080E04D0, 10);
    if ((u8)Callback_InvokeActor(a, a->update270))
        return;
    if (sub_0812F3CC(a))
        return;
    if (a->field2B0) {
        if (!Chase_TryRoute(a, *(void **)(group + 0x1FC), &a->position,
                            (struct ContactPosition *)(context + 0x6D8))) {
            Callback_InvokeActor(a, *(void **)(group + 0x224));
            return;
        }
        a->flags2D4 |= 0x200;
        if (a->flags2D4 & 0x80) {
            Chase_SetBehavior(a, 12);
            Chase_SetCtl(a, *(void **)(group + 0x48), 0x1A);
            *(u16 *)(group + 0x40) |= 4;
        } else {
            struct ContactPosition *p = *(struct ContactPosition **)(group + 0x200);
            Chase_SnapToRoute(p, group + 0x74);
            (*(struct ContactPosition **)(group + 0x200))->pad = 0;
            a->flags2CC |= 0x10;
            Chase_SetCtl(a, a->entryTarget228, 1);
            Chase_SetBehavior(a, 7);
            a->field2C2 = 0x20;
        }
    } else
        a->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080E4834);
#endif
