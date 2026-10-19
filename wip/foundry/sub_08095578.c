#define SceneActor_BeginLinkedReaction sub_08095578
#include "global.h"
#include "scene_actor_reset.h"
extern u8 *gUnk_02000580;
void sub_0807FC90(void);
void Sound_PlaySong(u32);
struct LinkedReactionContext {
    u8 unknown0[0x558];
    u32 flags;
    u8 unknown55C[0x5BB - 0x55C];
    u8 phase;
};
struct LinkedReactionCommand {
    u8 unknown0[10];
    u16 kind;
};
static inline u8 Blocked(struct SceneActorRegistryResetView *s)
{
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
void sub_08095578(void *unused, struct LinkedReactionCommand *cmd,
                  struct SceneActorRegistryResetView *s)
{
    u32 zero = Blocked(s);
    if (!zero) {
        u16 mask = 0x4000;
        zero = s->flags2D4 & mask;
        if (!zero) {
            u16 one = cmd->kind;
            if (one == 1) {
                struct LinkedReactionContext *p = s->context;
                void (*fn)(void) = sub_0807FC90;
                u8 code = 0x10;
                u8 zero8;
                s->field2B2 = one;
                s->field2B0 = zero;
                s->field2B5 = code;
                s->target = fn;
                zero8 = 0;
                s->field2C8 = zero;
                {
                    u32 *flags = &s->flags114;
                    u32 mask = -2;
                    *flags &= mask;
                }
                s->word104 = zero;
                {
                    u32 two = 2;
                    s->state2A8 = zero8;
                    s->phase2A9 = two;
                    s->behavior2AA = zero8;
                }
                s->counter2AC = zero;
                s->field2B1 = one;
                {
                    struct ContactCollider *b = &s->colliderB;
                    u32 mask = 4;
                    b->flags |= mask;
                    b = &s->colliderA;
                    b->flags |= mask;
                }
                {
                    u16 *flags = &s->flags2D6;
                    u32 mask = ~0x180;
                    *flags &= mask;
                }
                {
                    u32 mask = 0x80;
                    if (p->flags & mask) {
                        u32 three = 3;
                        gUnk_02000580[0x4F6] += three;
                        {
                            u32 mask = 0x80;
                            p->flags |= mask;
                        }
                        p->phase = three;
                    } else {
                        u32 three = 3;
                        gUnk_02000580[0x4F6] += three;
                        {
                            u32 mask = 0x80;
                            p->flags |= mask;
                        }
                        p->phase = three;
                    }
                }
                Sound_PlaySong(0x3CB);
            }
        }
    }
}
