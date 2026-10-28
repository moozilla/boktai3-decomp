#include "global.h"
/* SceneActor_BeginLinkedIdleReaction_0809C834: SceneActor_BeginLinkedIdleReaction variant with other offsets/flags and without the collider flag clear (provisional name) */
#include "scene_actor_reset.h"
#include "player.h"
void SceneActorControl_OnIdle(void);
void Sound_PlaySong(u32);
struct LinkedReactionContext {
    u8 unknown0[0x3D4];
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

static inline u8 LinkedFlag(struct LinkedReactionContext *p)
{
    u32 mask = 0x2;
    if (p->flags & mask)
        return 1;
    return 0;
}
void SceneActor_BeginLinkedIdleReaction_0809C834(struct SceneActorRegistryResetView *s)
{
    if (!Blocked(s) && LinkedFlag(s->context)) {
        u32 three = 3;
        u8 *state = &s->state2A8;
        u8 zero = 0;
        u8 one;
        void (*fn)(void);
        u8 code;
        *state = zero;
        s->phase2A9 = zero;
        s->behavior2AA = three;
        s->counter2AC = zero;
        {
            u8 *queued = &s->field2B1;
            one = 1;
            *queued = one;
        }
        fn = SceneActorControl_OnIdle;
        code = 0x10;
        s->field2B2 = one;
        s->field2B0 = zero;
        s->field2B5 = code;
        s->target = fn;
        s->field2C8 = zero;
        {
            u32 *flags = &s->flags114;
            u32 mask = -2;
            *flags &= mask;
        }
        s->word104 = zero;
    }
}
