#include "global.h"
#include "player.h"
void PlayerAction_BeginMode24(struct Player *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
s32 PlayerContact_ApplySyntheticContact(struct Player *, s32, u32, u32, u32, u32);
static inline u32 requestBlocked(u32 *flags, u32 mask)
{
    return *flags & mask;
}
s32 Player_BeginPositionRequest(struct Player *p, struct SceneVec *position,
                                struct SceneVec *target, s32 animationOffset, s32 side,
                                s32 parameter, s32 immediate, s32 request, u32 option)
{
    s32 result;
    if (p->kind != 1 || requestBlocked(&p->statusFlags, 0x40000))
        return -1;
    if (p->tetherMode != 0x18)
        PlayerAction_BeginMode24(p);
    p->position = *position;
    p->previousPosition = p->position;
    if (immediate) {
        p->animation.flags |= 1;
        p->movementRequestAnimated = 0;
    } else {
        p->animation.flags &= ~1;
        p->movementRequestAnimated = 1;
        p->side = side;
        Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x14) + animationOffset,
                            0x40, 1);
    }
    if (parameter > 0) {
        p->movementRequestParameter = parameter;
        p->movementRequestScale = 0x40;
    }
    if (request > 0) {
        result = PlayerContact_ApplySyntheticContact(p, request, 0, 0, 0, option);
        if (!p->movementRequestBlocked && target)
            p->movementRequestTarget = *target;
        return result;
    }
    return 0;
}
