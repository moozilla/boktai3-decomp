#include "global.h"
#define Dog_ResumeRouteAfterAction sub_080E4834
// confidence=low: resumes chase or route movement after an action completes.

#ifdef NONMATCHING
// fuzzy: 80.5%
// Register allocation, zero lifetimes and behavior-field address reuse still differ.
#include "scene_actor_chase.h"
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void sub_080E04D0(void);
void Dog_ResumeRouteAfterAction(struct SceneActorRegistryResetView *a)
{
    u8 *group = (u8 *)&a->group;
    u8 *context = a->context;
    if (Chase_TakeArrived(a))
        Chase_SetCtl(a, sub_080E04D0, 10);
    if ((u8)Callback_InvokeActor(a, a->update270))
        return;
    if (sub_0812F3CC(a))
        return;
    if (a->field2B0) {
        if (!Chase_TryRoute(a, *(void **)(group + 0x1FC), &a->position,
                            (void *)(context + 0x6D8))) {
            Callback_InvokeActor(a, *(void **)(group + 0x224));
            return;
        }
        {
            u32 mask = 0x200;
            u16 *flags = &a->flags2D4;
            u32 value = mask | *flags;
            *flags = value;
            if (value & 0x80) {
                Chase_SetBehavior(a, 12);
                Chase_SetCtl(a, *(void **)(group + 0x48), 26);
                {
                    u16 *field = (u16 *)(group + 0x40);
                    u32 mask = 4;
                    *field = mask | *field;
                }
            } else {
                Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
                (*(struct ContactPosition **)(group + 0x200))->pad = 0;
                {
                    u32 mask = 16;
                    a->flags2CC |= mask;
                }
                Chase_SetCtl(a, a->entryTarget228, 1);
                Chase_SetBehavior(a, 7);
                a->field2C2 = 32;
            }
        }
    } else
        a->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080E4834);
#endif
