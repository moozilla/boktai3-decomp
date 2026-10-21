// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
#define F(p, o, T) (*(T *)((u8 *)(p) + (o)))
void Sound_PlaySong(u32);
void sub_081218C0(void *, void *, u32, u32, u32);
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
#ifdef NONMATCHING
// fuzzy: 90.7%
// confidence=low: Spider callback; plays a footstep sound on an animation frame and begins contact after n ticks.
void sub_080A28A8(struct SceneActorRegistryResetView *a, s32 n)
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
            sub_081218C0(transform, v, a->fieldB6, a->fieldBE, a->fieldC6);
    }
    {
        u32 q = 0x2B3;
        register s32 nn asm("r5") = n;
        if (F(a, q, u8) && nn > 60) {
            register u16 *fl asm("r6") = &a->flags2D4;
            register u32 z4 asm("r4");
            register u32 z3 asm("r3");
            void *t;
            {
                u32 m = ~0x100;
                u32 v = *fl;
                m &= v;
                z4 = 0;
                z3 = 0;
                *fl = m;
            }
            q -= 0xB;
            F(a, q, u8) = z4;
            nn = 0x2A9;
            F(a, nn, u8) = z4;
            q += 2;
            F(a, q, u8) = 12;
            nn += 3;
            F(a, nn, u32) = z3;
            {
                u32 b = 0x2B1;
                F(a, b, u8) = 1;
            }
            t = F(nav, 0x48, void *);
            nn += 6;
            F(a, nn, u8) = 1;
            {
                u32 b = 0x2B0;
                u32 k = 26;
                F(a, b, u8) = z4;
                nn += 3;
                F(a, nn, u8) = k;
                b -= 0x94;
                F(a, b, void *) = t;
            }
            nn += 0x13;
            F(a, nn, u16) = z3;

            {
                u32 *p = &a->flags114;
                u32 m = ~1;
                *p &= m;
            }
            a->word104 = z3;
            sub_080A4798(a);
            SceneActorMovement_ResolveRoutePosition(a);
            {
                u16 *p = (void *)((u8 *)a + 0x414);
                u32 m = 4;
                u32 v = *p;
                m |= v;
                *p = m;
            }
            {
                u32 m = 128;
                u32 v = *fl;
                m |= v;
                *fl = m;
            }
            {
                u32 v = F(a, 0x416, u16);
                F(a, 0x418, u16) = v;
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080A28A8);
#endif
