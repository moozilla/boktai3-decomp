#include "global.h"
#include "player.h"
#include "ram.h"
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_AddControlFlags(struct Player *, s32);
static inline u32 requestBlocked(u32 *flags, u32 mask)
{
    return *flags & mask;
}
s32 Player_ResetPositionState(struct Player *p, struct SceneVec *position)
{
    if (p->kind != 1 || requestBlocked(&p->statusFlags, 0x40000))
        return 0;
    if (p->tetherMode != 0x1A)
        ActorSequence_Begin(p, 0x1A, 0);
    p->position = *position;
    Player_AddControlFlags(p, 8);
    p->collider.flags |= 4;
    p->directionFrame = 0;
    p->contactSpecial = 0;
    p->positionResetTimer = 0;
    p->positionResetCounter = 0;
    p->positionResetState = 0;
    p->particleEmissionCount = 0;
    gPlayerPendingActor = 0;
    gPlayerNotificationPending = 0;
    return 1;
}
