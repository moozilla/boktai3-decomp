#include "engine/video/sprite.h"
#include "engine/task.h"

struct TimedSprite {
    struct Task task;
    struct DisplaySequenceSprite sprite;
    struct SpriteRenderResource resource;
    u16 age;
    u8 unknown62[2];
    s32 sinkSpeed;
};
void SingletonSlot7_InitImpactSprite(struct DisplaySequenceSprite *, struct SpriteRenderResource *,
                                     struct ContactPosition *, s32);
u32 TimedSprite_Init(struct TimedSprite *p, struct ContactPosition *pos, s32 frame, s32 sinkSpeed)
{
    SingletonSlot7_InitImpactSprite(&p->sprite, &p->resource, pos, frame);
    p->age = 0;
    p->sinkSpeed = sinkSpeed;
    return 0;
}
