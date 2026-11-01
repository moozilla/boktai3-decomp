#include "scene_actor_chase.h"
#include "global.h"
#include "scene_actor_control_phase.h"
#define F(p, o, T) (*(T *)((u8 *)(p) + (o)))
void Sound_PlaySong(u32);
void SceneColliderEffect_SpawnAimedSprite(void *, void *, u32, u32, u32);
void sub_080A4798(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
static inline void Control(struct SceneActorRegistryResetView *a, void *c, u32 k)
{
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = k;
    a->target = c;
    a->field2C8 = 0;
    {
        u32 *p = &a->flags114;
        u32 m = ~1;
        *p &= m;
    }
    a->word104 = 0;
}
static inline void State(struct SceneActorRegistryResetView *a, u32 k)
{
    a->state2A8 = 0;
    a->phase2A9 = 0;
    a->behavior2AA = k;
    a->counter2AC = 0;
    a->field2B1 = 1;
}

// confidence=low: windup emits a contact cue before entering the pursuit phase.
void Spider_UpdateContactWindup(struct SceneActorRegistryResetView *a, s32 n)
{
    u8 *anim = (u8 *)a->drawable + 0x48;
    u8 *nav = (u8 *)a + 0x3D4;
    u8 *c = (u8 *)SceneActorControl_GetState(a);
    struct SceneActorBody *transform = &a->body;
    if (SceneActorControl_TakeFlag(&a->field2B2)) {
        {
            u32 m = 0x100;
            a->flags2D4 = m | a->flags2D4;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)a, 5);
        {
            u32 *p = &a->flags2CC;
            u32 m = ~0x4000;
            *p &= m;
        }
        F(c, 0x624, void *) = a->tracking.target;
    }
    if (F(anim, 8, u16) == 3 && F(anim, 0xE, u16) == 0) {
        void *v;
        Sound_PlaySong(0x114);
        v = F(c, 0x624, void *);
        if (v)
            SceneColliderEffect_SpawnAimedSprite(transform, v, a->attacks.first[1],
                                                 a->attacks.second[1], a->attacks.parameter[1]);
    }
    if (a->field2B3 && n > 60) {
        {
            struct ChaseFlags40 *p = (void *)&a->flags2D4;
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
            mask |= a->flags2D4;
            a->flags2D4 = mask;
        }
        a->group.common.gate.counter = a->group.common.gate.duration;
    }
}
