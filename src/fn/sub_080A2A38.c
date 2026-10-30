// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
/* SceneActorReaction_ResumeEntryTarget: tentative */

static inline u8 TakePrepared(struct SceneActorRegistryResetView *actor)
{
    register u32 offset asm("r2") = 0x2B2;
    u8 *field;
    asm("" : "+r"(offset));
    field = (u8 *)actor + offset;
    if (*field) {
        *field = 0;
        return 1;
    }
    return 0;
}

void SceneActorReaction_ResumeEntryTarget(struct SceneActorRegistryResetView *actor)
{
    register struct SceneActorReactionGroup *group asm("r5") = (void *)&actor->group.common;
    register u32 timer asm("r5");
    TakePrepared(actor);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
    {
        register u16 *tp asm("r0") = &group->timer418;
        timer = *tp;
    }
    if (timer == 0) {
        register u32 offset asm("r6");
        register u32 off2 asm("r2");
        u8 one, zero, seven;
        u32 mask, clr;
        void *saved;
        mask = 0x81;
        {
            register u32 o3 asm("r3") = 0x2D4;
            register u16 *fp asm("r2");
            u16 v;
            asm("" : "+r"(o3));
            fp = (u16 *)((u8 *)actor + o3);
            v = *fp & -mask;
            zero = 0;
            *fp = v;
        }
        seven = 7;
        offset = 0x2A8;
        *((u8 *)actor + offset) = zero;
        off2 = 0x2A9;
        *((u8 *)actor + off2) = zero;
        offset += 2;
        *((u8 *)actor + offset) = seven;
        actor->counter2AC = timer;
        one = 1;
        off2 += 8;
        {
            register u8 *p asm("r0") = (u8 *)actor + off2;
            one = 1;
            asm("" : "+r"(one));
            *p = one;
        }
        offset -= 0x82;
        saved = *(void **)((u8 *)actor + offset);
        offset += 0x8A;
        *((u8 *)actor + offset) = one;
        offset -= 2;
        *((u8 *)actor + offset) = zero;
        actor->field2B5 = one;
        offset -= 0x94;
        {
            register void **dst asm("r0") = (void **)((u8 *)actor + offset);
            *dst = saved;
        }
        actor->field2C8 = timer;
        {
            u32 *f = &actor->flags114;
            clr = ~1;
            *f &= clr;
        }
        offset = 0x104;
        asm("" : "+r"(offset));
        {
            register u32 *w asm("r0") = (u32 *)((u8 *)actor + offset);
            *w = timer;
        }
    }
}
