// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
#define F(p, o, T) (*(T *)((u8 *)(p) + (o)))
void ScaledShadow_Enable(void *);
void ScaledShadow_Destroy(void *);
void SceneActorReaction_RestoreAlternateAfterDelay(void);
// confidence=low: Spider callback; starts a shadowed state, and on a control flag restores the default one.
void sub_080A24D0(struct SceneActorRegistryResetView *a)
{
    register u8 *c asm("r8") = (u8 *)SceneActorControl_GetState(a);
    if (SceneActorControl_TakeFlag(&a->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
        ScaledShadow_Enable((u8 *)a + 0x360);
        {
            u32 m = 0x800;
            a->flags2C0 = m | a->flags2C0;
        }
        {
            struct ContactCollider *k = &a->colliderB;
            k->flags |= 4;
        }
        {
            struct ContactCollider *k = &a->colliderA;
            k->flags |= 4;
        }
    }
    if (a->flags2D0 & 0x10) {
        if (F(a, 0x360, u8))
            ScaledShadow_Destroy((u8 *)a + 0x360);
        {
            u32 m = ~0x800;
            a->flags2C0 &= m;
        }
        {
            struct ContactCollider *k = &a->colliderB;
            u32 m = ~4;
            k->flags &= m;
        }
        {
            struct ContactCollider *k = &a->colliderA;
            u32 m = ~4;
            k->flags &= m;
        }
        {
            u8 *q = (u8 *)a + 0x460;
            void *saved = a->savedTarget;
            F(q, 0xC, u32) = 20;
            F(q, 0x10, void *) = saved;
            F(q, 8, u8) = 0;
            F(q, 9, u8) = 0;
            F(q, 0xA, u8) = 5;
        }
        {
            register u32 off asm("r7");
            u32 z4 = 0;
            u32 z5 = 0;
            u32 one;
            u32 k = 0xE;
            u32 two = 2;
            u32 b2 = 0x2B2;
            one = 1;
            F(a, b2, u8) = one;
            off = 0x2B0;
            F(a, off, u8) = z4;
            off += 5;
            F(a, off, u8) = k;
            a->target = SceneActorReaction_RestoreAlternateAfterDelay;
            a->field2C8 = z5;
            {
                u32 *p = &a->flags114;
                s32 m = -two;
                *p &= m;
                a->word104 = z5;
                off = 0x2A8;
                F(a, off, u8) = 3;
                {
                    u32 ph = 0x2A9;
                    F(a, ph, u8) = one;
                    off += 2;
                    F(a, off, u8) = z4;
                    ph += 3;
                    F(a, ph, u32) = z5;
                }
                off += 7;
                F(a, off, u8) = one;
                {
                    u16 *fp = (u16 *)(c + 0x62C);
                    u32 v = *fp;
                    m &= v;
                    *fp = m;
                }
            }
        }
    }
}
