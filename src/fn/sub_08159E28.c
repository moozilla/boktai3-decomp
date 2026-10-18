#define Player_ResetPositionState sub_08159E28
#include "global.h"
#include "player.h"
extern u32 gUnk_0200054C;
extern u16 gUnk_02000594;
void sub_0813AFEC(struct Player *, s32, s32);
void Player_AddControlFlags(struct Player *, s32);
static inline u32 requestBlocked(u32 *flags, u32 mask)
{
    return *flags & mask;
}
s32 sub_08159E28(struct Player *p, struct SceneVec *position)
{
    if (p->kind != 1 || requestBlocked(&p->statusFlags, 0x40000))
        return 0;
    if (p->tetherMode != 0x1A)
        sub_0813AFEC(p, 0x1A, 0);
    p->position = *position;
    Player_AddControlFlags(p, 8);
    p->collider.flags |= 4;
    p->directionFrame = 0;
    p->contactSpecial = 0;
    p->positionResetTimer = 0;
    p->positionResetCounter = 0;
    p->positionResetState = 0;
    p->particleEmissionCount = 0;
    gUnk_0200054C = 0;
    gUnk_02000594 = 0;
    return 1;
}
