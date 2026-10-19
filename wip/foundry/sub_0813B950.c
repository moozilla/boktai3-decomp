#include "global.h"
#define ActorEffect_UpdateBriefAnimation sub_0813B950
struct ActorBriefEffect {
    u8 pad00[0x744];
    void *resource;
    u32 flags;
    u8 pad74C[0x24];
    u8 active;
    u8 frame;
    u8 baseFrame;
};
struct LevelAdvanceParticle;
void sub_08217EEC(struct LevelAdvanceParticle *, void *, u32);
void sub_0813B950(struct ActorBriefEffect *actor)
{
    if (actor->active) {
        actor->frame++;
        if (actor->frame > 5) {
            actor->flags |= 1;
            actor->active = 0;
        } else {
            sub_08217EEC((struct LevelAdvanceParticle *)&actor->flags, actor->resource,
                        (actor->frame >> 2) + actor->baseFrame);
        }
    }
}
