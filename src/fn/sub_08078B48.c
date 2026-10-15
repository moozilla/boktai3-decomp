#include "scene_actor_behavior.h"
#define SceneActorBehavior_TestDefeated sub_08078B48

void SceneActorBehavior_RecordDefeat(struct SceneActorBehaviorView *);

static inline u8 TestMask(struct SceneActorBehaviorGateView *s, u32 mask)
{
    if (s->flags & mask)
        return TRUE;
    return FALSE;
}

static inline u32 OrV(u16 m, u16 *f)
{
    return m | *f;
}

u32 SceneActorBehavior_TestDefeated(struct SceneActorBehaviorView *actor)
{
    struct SceneActorBehaviorGateView *sprite;
    if (actor->counter304 > 0) {
        u8 m;
        SceneActorBehavior_RecordDefeat(actor);
        {
            s32 v = OrV(0x80, &actor->flags2D6) & ~0x100;
            *&actor->flags2D6 = v;
        }
        sprite = &actor->gate;
        m = 0x10;
        sprite->flags |= m;
        return 1;
    }
    sprite = &actor->gate;
    if (TestMask(sprite, 2)) {
        s32 m = ~0x10;
        sprite->flags &= m;
    }
    return 0;
}
