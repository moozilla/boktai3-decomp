// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"
void SceneActorControl_OnIdle(void);
void Sound_PlaySong(u32);
static inline u8 Blocked(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *s = input;
    if (!s)
        goto zero;
    {
        u32 mask = 0x40;
        if (s->flags2D4 & mask)
            goto one;
    }
    {
        u32 mask = 0x20000;
        if (s->flags2D0 & mask)
            goto one;
    }
    if (s->health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}
static inline u8 Test(u32 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline void ColliderFlag(struct ContactCollider *c)
{
    c->flags |= 4;
}
// confidence=low: contact type 1 interrupts control and starts a linked-player reaction.
void Ghost_BeginContactReaction(void *unused, struct ContactCollider *contact,
                                struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *s = input;
    if (!Blocked(s)) {
        register u32 mask asm("r1") = 0x4000;
        u16 zero = s->flags2D4 & mask;
        if (!zero && contact->contactFlags == 1) {
            register u8 *p asm("r6") = s->context;
            void *fn = SceneActorControl_OnIdle;
            u8 code = 0x10;
            u8 one = 1;
            u8 zero8;
            u32 *flags;
            u32 two;

            s->field2B2 = one;
            s->field2B0 = zero;
            s->field2B5 = code;
            s->target = fn;
            zero8 = 0;
            s->field2C8 = zero;
            flags = &s->flags114;
            {
                u32 mask = -2;
                *flags &= mask;
            }
            s->word104 = zero;
            two = 2;
            s->state2A8 = zero8;
            s->phase2A9 = two;
            s->behavior2AA = zero8;
            s->counter2AC = zero;
            s->field2B1 = one;
            ColliderFlag(&s->colliderB);
            ColliderFlag(&s->colliderA);
            {
                u16 *f = &s->flags2D6;
                u32 mask = ~0x180;
                *f &= mask;
            }
            {
                u32 mask = 0x80;
                register u32 offset asm("r2") = 0x558;
                u32 test;
                u32 *field;
                field = (u32 *)(p + offset);
                test = *field & mask;
                asm volatile("" : : "r"(test) : "memory");
            }
            {
                u8 *player = (*(u8 **)gPlayers);
                u32 three;
                three = 3;
                player[0x4F6] += 3;
                {
                    register u32 mask asm("r2") = 0x80;
                    u32 offset = 0x558;
                    u32 *flags;
                    flags = (u32 *)(p + offset);
                    *flags |= mask;
                }
                {
                    register u8 *field asm("r0") = p + 0x5BB;
                    *field = three;
                }
            }
            Sound_PlaySong(0x3CB);
        }
    }
}
