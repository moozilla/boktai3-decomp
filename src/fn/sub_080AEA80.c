// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_animation.h"
#include "engine.h"

void Vorn_SelectRetreatRoute(struct SceneActorAnimationView *, u32);
static inline u8 ConsumeFlag(struct SceneActorAnimationView *p)
{
    if (p->directionalEntryFlag) {
        p->directionalEntryFlag = 0;
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
    register u16 *table asm("r4");
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    offset = gRandomIndex * 2;
    table = gUnk_0203B400;
    return *(u8 *)(offset + (u32)table);
}
void SceneActorPhase_SelectLinkedAnimation(struct SceneActorAnimationView *p)
{
    struct SceneActorAnimationLink *link = p->link;
    if (!ConsumeFlag(p)) {
        if (LinkActive(link)) {
            if (p->phaseTimer > link->phaseTimer) {
                u32 state = 0x17;
                p->phase = 0;
                p->subphase = 0;
                p->nextPhase = state;
                p->phaseCounter = 0;
                p->phaseReady = 1;
                p->nextDirection = (AngleIndex(p->facingAngle) + 3) & 7;
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
