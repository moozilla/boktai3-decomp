#include "global.h"
#include "scene_actor_animation.h"
#include "engine.h"

void Vorn_SelectRetreatRoute(struct RoomActor *, u32);
static inline u8 ConsumeFlag(struct RoomActor *p)
{
    if (p->field2B2) {
        p->field2B2 = 0;
        return 1;
    }
    return 0;
}
static inline u8 LinkActive(struct SceneActorAnimationLink *p)
{
    u32 mask = 0x40;
    if (p->flags & mask)
        return 1;
    return 0;
}
static inline s32 AngleIndex(s32 angle)
{
    if (angle >= 0)
        return angle >> 5;
    return -((-angle) >> 5);
}
static inline s32 RandomByte(void)
{
    u32 offset;
    u32 *index = &gRandomIndex;
    u32 next = *index + 1;
    u32 mask = 1023;
    *index = next & mask;
    offset = *index * 2;
    return *(const u8 *)(offset + RANDOM_TABLE_ADDR);
}
void SceneActorPhase_SelectLinkedAnimation(struct RoomActor *p)
{
    struct SceneActorAnimationLink *link = p->context;
    if (!ConsumeFlag(p)) {
        if (LinkActive(link)) {
            if (p->tracking.distance > link->phaseTimer) {
                u32 state = 0x17;
                p->state2A8 = 0;
                p->phase2A9 = 0;
                p->behavior2AA = state;
                p->counter2AC = 0;
                p->field2B1 = 1;
                p->nextDirection = (AngleIndex(p->tracking.heading) + 3) & 7;
            } else {
                if (RandomByte() <= 0x1f)
                    Vorn_SelectRetreatRoute(p, 1);
                else
                    SceneActorAnimation_SelectDirectionalEntry(p, 0);
            }
        } else
            SceneActorAnimation_SelectDirectionalEntry(p, 1);
    }
}
