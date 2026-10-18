#define Player_BeginInterpolatedMovement   sub_08159C0C
#define Player_PrepareInterpolatedMovement Player_InitState25
#include "global.h"
#include "player.h"
void Player_InitState25(struct Player *);
s32 sub_081419D4(u32, s32);
s32 sub_0813AE18(struct Player *, s32, s32, s32);
s32 sub_0815E710(struct Player *, s32, u32, u32, u32, u32);
static inline u32 requestBlocked(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline void enableAnimation(struct PlayerAnimation *a)
{
    a->flags &= ~1;
}
s32 sub_08159C0C(struct Player *p, struct SceneVec *destination, s32 duration,
                 struct SceneVec *target, u32 unused, s32 request, u32 option)
{
    s32 result;
    if (p->kind != 1 || requestBlocked(&p->statusFlags, 0x40000))
        return -1;
    if (p->tetherMode != 0x18)
        Player_InitState25(p);
    p->movementDestination = *destination;
    p->movementDuration = duration;
    p->movementStart = p->position;
    enableAnimation(&p->animation);
    sub_0813AE18(p, sub_081419D4(p->actionMode, 0x31), 0x40, 1);
    p->actionEighteenParameter = 500;
    p->movementRequestScale = 0x40;
    if (request > 0) {
        result = sub_0815E710(p, request, 0, 0, 0, option);
        if (!p->movementRequestBlocked && target)
            p->movementRequestTarget = *target;
        return result;
    }
    return 0;
}
