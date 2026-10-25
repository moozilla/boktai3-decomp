// MATCHING_CONCESSION: register-pinning
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

// confidence=low: Spider callback; plays a footstep sound on an animation frame and begins contact after n ticks.
// confidence=low: windup emits a contact cue before entering the pursuit phase.
#define Spider_UpdateContactWindup sub_080A28A8
void Spider_UpdateContactWindup(struct SceneActorRegistryResetView *a, s32 n)
{
    u8 *anim = (u8 *)a->drawable + 0x48;
    u8 *nav = (u8 *)a + 0x3D4;
    u8 *c = (u8 *)SceneActorControl_GetState(a);
    register u8 *transform asm("r6") = (u8 *)a + 0x50;
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
        F(c, 0x624, u32) = a->fieldDC;
    }
    if (F(anim, 8, u16) == 3 && F(anim, 0xE, u16) == 0) {
        void *v;
        Sound_PlaySong(0x114);
        v = F(c, 0x624, void *);
        if (v)
            SceneColliderEffect_SpawnAimedSprite(transform, v, a->fieldB6, a->fieldBE, a->fieldC6);
    }
    {
        u32 q = 0x2B3;
        if (F(a, q, u8)) {
            register s32 nn asm("r5") = n;
            asm("" : "+r"(nn));
            if (nn > 60) {
                u16 *fl = &a->flags2D4;
                u32 z4;
                register u32 z3 asm("r3");
                void *t;
                register u32 kind asm("r2");
                u32 behavior;
                {
                    u32 m = ~0x100;
                    u32 v = *fl;
                    m &= v;
                    z4 = 0;
                    z3 = 0;
                    *fl = m;
                }
                behavior = 12;
                q -= 0xB;
                F(a, q, u8) = z4;
                nn = 0x2A9;
                F(a, nn, u8) = z4;
                q += 2;
                F(a, q, u8) = behavior;
                nn += 3;
                F(a, nn, u32) = z3;
                {
                    u32 b = 0x2B1;
                    F(a, b, u8) = 1;
                }
                t = F(nav, 0x48, void *);
                kind = 26;
                asm("" : "+r"(kind));
                nn += 6;
                F(a, nn, u8) = 1;
                {
                    register u32 b asm("r1") = 0x2B0;
                    u32 k = kind;
                    F(a, b, u8) = z4;
                    nn += 3;
                    F(a, nn, u8) = k;
                    b -= 0x94;
                    {
                        register void **address asm("r0") = (void **)((u8 *)a + b);
                        register void *callback asm("r2") = t;
                        asm("" : "+r"(address), "+r"(callback));
                        *address = callback;
                    }
                }
                nn += 0x13;
                F(a, nn, u16) = z3;
                asm("" : : "r"(nn));

                {
                    u32 *p = &a->flags114;
                    u32 m = ~1;
                    *p &= m;
                }
                a->word104 = z3;
                sub_080A4798(a);
                SceneActorMovement_ResolveRoutePosition(a);
                {
                    register u32 offset asm("r5") = 0x414;
                    u16 *p;
                    u32 m;
                    u32 v;
                    p = (void *)((u8 *)a + offset);
                    m = 4;
                    v = *p;
                    m |= v;
                    *p = m;
                    asm("" : : "r"(offset));
                }
                {
                    u32 m = 128;
                    u32 v = *fl;
                    m |= v;
                    *fl = m;
                }
                {
                    register u32 offset asm("r1") = 0x416;
                    register u16 *field asm("r0");
                    u32 v;
                    asm("" : "+r"(offset));
                    field = (void *)((u8 *)a + offset);
                    v = *field;
                    F(a, 0x418, u16) = v;
                }
            }
        }
    }
}
