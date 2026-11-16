#include "scene_actor_chase.h"
#include "global.h"
#include "scene_actor_control_phase.h"
#define F(p, o, T) (*(T *)((u8 *)(p) + (o)))
void Sound_PlaySong(u32);
void SceneColliderEffect_SpawnAimedSprite(void *, void *, u32, u32, u32);
void sub_080A4798(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
static inline void Control(struct ChaseActor *a, void *c, u32 k)
{
    a->base.field2B2 = 1;
    a->base.field2B0 = 0;
    a->base.field2B5 = k;
    a->base.target = c;
    a->base.field2C8 = 0;
    {
        u32 *p = &a->base.flags114;
        u32 m = ~1;
        *p &= m;
    }
    a->base.word104 = 0;
}
static inline void State(struct ChaseActor *a, u32 k)
{
    a->base.state2A8 = 0;
    a->base.phase2A9 = 0;
    a->base.behavior2AA = k;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}

// confidence=low: windup emits a contact cue before entering the pursuit phase.
void Spider_UpdateContactWindup(struct ChaseActor *a, s32 n)
{
    u8 *anim = (u8 *)a->base.drawable + 0x48;
    u8 *nav = (u8 *)a + 0x3D4;
    u8 *c = (u8 *)SceneActorControl_GetState(a);
    struct SceneActorBody *transform = &a->base.body;
    if (SceneActorControl_TakeFlag(&a->base.field2B2)) {
        {
            u32 m = 0x100;
            a->base.flags2D4 = m | a->base.flags2D4;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)a, 5);
        {
            u32 *p = &a->base.flags2CC;
            u32 m = ~0x4000;
            *p &= m;
        }
        F(c, 0x624, void *) = a->base.tracking.target;
    }
    if (F(anim, 8, u16) == 3 && F(anim, 0xE, u16) == 0) {
        void *v;
        Sound_PlaySong(0x114);
        v = F(c, 0x624, void *);
        if (v)
            SceneColliderEffect_SpawnAimedSprite(transform, v, a->base.attacks.first[1],
                                                 a->base.attacks.second[1],
                                                 a->base.attacks.parameter[1]);
    }
    if (a->base.restart && n > 60) {
        {
            struct ChaseFlags40 *p = (void *)&a->base.flags2D4;
            u32 mask = ~0x100;
            p->bits &= mask;
        }
        State(a, 12);
        Control(a, F(nav, 0x48, void *), 26);
        sub_080A4798(a);
        SceneActorMovement_ResolveRoutePosition(a);
        {
            s16 *ptr = &a->group.common.gate.flags;
            u16 mask = 4;
            ((struct ChaseFlags40 *)ptr)->bits = mask | ((struct ChaseFlags40 *)ptr)->bits;
        }
        {
            u32 mask = 128;
            mask |= a->base.flags2D4;
            a->base.flags2D4 = mask;
        }
        a->group.common.gate.counter = a->group.common.gate.duration;
    }
}
