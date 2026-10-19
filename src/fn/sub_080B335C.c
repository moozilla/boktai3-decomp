
#ifdef NONMATCHING
// fuzzy: 83.6%
// foundry draft (worker best 81.7%); what differs is mostly register allocation and instruction order
#define SceneActor_IsFacingNearbyTarget sub_080B335C
#include "global.h"
#include "scene_actor_reset.h"
static inline u32 TestFlag(u16 *p, u32 mask) { return *p & mask; }
s32 sub_080B335C(struct SceneActorRegistryResetView *p)
{
    struct SceneActorRegistryResetView *q = p->context;
    s32 difference, absolute;
    if (q->field6E8) return 0;
    if (TestFlag(&p->flags2D4, 0x100)) return 0;
    if (p->distanceF0 > 0xFFFF) return 0;
    difference = p->fieldE9 - p->direction;
    if (difference > 0x80) difference -= 0x100;
    else if (difference < -0x80) difference += 0x100;
    absolute = difference;
    if (absolute < 0) absolute = -absolute;
    if (absolute > 0x12) return 0;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080B335C);
#endif
