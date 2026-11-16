#include "scene_actor_reset.h"

void TentacleBody_UpdateSpreadWindup(void);
void TentacleActor_SelectWanderRoute(struct SceneActorRegistryResetView *);
static inline u8 TakeQueuedFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
void SceneActorPlacement_UpdateFlaggedResponse(struct SceneActorRegistryResetView *p)
{
    s32 savedOffset;
    if (TakeQueuedFlag(&p->base.field2B1)) {
        void (*control)(void) = TentacleBody_UpdateSpreadWindup;
        u8 kind = 38;
        u32 zero;
        u32 *flags;
        s32 mask;
        {
            u8 *taken = &p->base.field2B2;
            zero = 0;
            *taken = 1;
        }
        do {
            u32 offset = 0x2B0;
            do {
                *((u8 *)p + offset) = zero;
            } while (0);
            offset += 5;
            *((u8 *)p + (savedOffset = offset)) = kind;
            p->base.target = (void *)control;
            offset += 0x13;
            *(u16 *)(offset + (u32)p) = zero;
        } while (0);
        flags = &p->base.flags114;
        mask = -2;
        *flags &= mask;
        p->base.word104 = zero;
        {
            u32 mask = 0x100000;
            u32 *flags = &p->base.flags2D0;
            *flags |= mask;
        }
        Sound_PlaySong(0x19B);
    }
    savedOffset = 39;
    if (p->base.field2B5 == savedOffset &&
        (u8)Callback_InvokeActor(p, p->base.contactQuery.opaque)) {
        u32 two = 2;
        u8 *field = &p->base.state2A8;
        u32 zero = 0;
        *field = zero;
        p->base.phase2A9 = two;
        p->base.behavior2AA = zero;
        p->base.counter2AC = zero;
        p->base.field2B1 = 1;
    } else if (p->base.field2B0) {
        u32 two;
        u32 zero;
        u8 *field;
        TentacleActor_SelectWanderRoute(p);
        two = 2;
        field = &p->base.state2A8;
        zero = 0;
        *field = zero;
        p->base.phase2A9 = zero;
        p->base.behavior2AA = two;
        p->base.counter2AC = zero;
        p->base.field2B1 = 1;
        {
            u32 *flags = &p->base.flags2D0;
            u32 mask = ~0x100000;
            *flags &= mask;
        }
    } else {
        p->base.counter2AC++;
    }
}
