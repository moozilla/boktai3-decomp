#include "engine/shared/global.h"
#include "engine/shared/player.h"
u32 Player_GetConfiguredActionPower(struct Player *);
u32 Player_CollectCallbackFlags(struct Player *);
void Player_ConsumeActionResource(struct Player *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void sub_0823E704(struct Player *p)
{
    struct PlayerStatModifierContext *q = &p->statModifierContext;
    u32 resource, flags;
    q->contactOrigin = p->position;
    q->contactOrigin.y += 230;
    q->contactHeading = ((p->facing + 5) & 7) << 5;
    resource = Player_GetConfiguredActionPower(p);
    flags = Player_CollectCallbackFlags(p);
    if (p->actionMode == 1) {
        q->motionSpeed = 29;
        q->windupFrames = 7;
        q->swingFrames = 4;
        q->holdFrames = 6;
        flags |= 0x40000;
    } else {
        q->motionSpeed = 19;
        q->windupFrames = 11;
        q->swingFrames = 6;
        q->holdFrames = 9;
    }
    if (q->savedActionIndex >= 0) {
        flags |= 1 << q->savedActionIndex;
        Player_ConsumeActionResource(p);
    }
    Collider_SetAttackProperties(&q->attackCollider, resource, q->actionTimer, 4, flags,
                                 q->contactParameter);
    q->attackCollider.direction = q->contactHeading;
    q->contactTimer = 0;
}
