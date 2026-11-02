#include "global.h"
#include "player.h"
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
        q->contactExtentA = 29;
        q->contactCounter = 7;
        q->contactExtentB = 4;
        q->contactExtentC = 6;
        flags |= 0x40000;
    } else {
        q->contactExtentA = 19;
        q->contactCounter = 11;
        q->contactExtentB = 6;
        q->contactExtentC = 9;
    }
    if (q->savedActionIndex >= 0) {
        flags |= 1u << q->savedActionIndex;
        Player_ConsumeActionResource(p);
    }
    Collider_SetAttackProperties(&q->drain, resource, q->actionTimer, 4, flags,
                                 q->contactParameter);
    q->contactDirection = q->contactHeading;
    q->contactTimer = 0;
}
