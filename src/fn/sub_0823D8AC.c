#include "global.h"
#include "player.h"

#define Player_StartContactAttack sub_0823D8AC

u32 Player_GetConfiguredActionPower(struct Player *);
u32 Player_CollectCallbackFlags(struct Player *);
void Player_ConsumeActionResource(struct Player *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

void Player_StartContactAttack(struct Player *p)
{
    struct PlayerStatModifierContext *q = &p->statModifierContext;
    u32 power, flags, param;
    q->contactOrigin = *(struct SceneVec *)&p->position;
    q->contactOrigin.y += 200;
    q->contactHeading = ((p->facing + 5) & 7) << 5;
    power = Player_GetConfiguredActionPower(p);
    flags = Player_CollectCallbackFlags(p);
    param = q->contactParameter;
    q->contactExtentA = 8;
    q->contactExtentB = 8;
    q->contactCounter = 4;
    if (q->savedActionIndex >= 0) {
        flags |= 1U << q->savedActionIndex;
        Player_ConsumeActionResource(p);
    }
    Collider_SetAttackProperties((u8 *)q + 0x48, power, q->actionTimer, 1, flags, param);
    q->contactDirection = q->contactHeading;
    q->contactTimer = 0;
}
