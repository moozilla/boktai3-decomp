#include "monsters/centipede/centipede_body.h"

static inline u8 Pred(struct TentacleBody *body)
{
    u32 mask = 2;
    if (body->attackFlags & mask)
        return 1;
    return 0;
}
void Centipede_EnterHitReaction(struct ChaseActor *actor)
{
    struct TentacleBody *body = actor->base.context;
    u8 active = Pred(body);
    u16 *flags = &body->attackFlags;
    if (active) {
        u32 four = 4;
        actor->base.state2A8 = 0;
        actor->base.phase2A9 = 0;
        actor->base.behavior2AA = four;
        actor->base.counter2AC = 0;
        actor->base.field2B1 = 1;
        {
            u32 *status = &actor->base.statusFlags;
            u32 mask = ~0x100000;
            *status &= mask;
        }
        body->hitReactionTimer = 180;
        actor->base.field2E8 = 2;
        four |= *flags;
        *flags = four;
    }
    {
        u32 mask = ~2;
        *flags &= mask;
    }
}
