#include "global.h"
#include "background_controller.h"
struct WorldPosition {
    s32 x, y, z;
};
struct ScreenPosition {
    s16 x, y, z, unused;
};
struct ScreenMovingActor {
    u8 pad0[0x268];
    struct WorldPosition position;
    u8 pad274[4];
    struct ScreenPosition screen;
    s32 horizontalSpeed;
    s32 horizontalAcceleration;
    u8 pad288[0x2AC - 0x288];
    s32 targetZ;
    u8 pad2B0[0x3A0 - 0x2B0];
    s32 playerIndex;
};
struct DisplayCamera {
    u8 pad[0x44];
    s32 x;
};
s32 Player_GetHorizontalVelocity(s32);
s32 ActorMotion_AdvanceTowardScreenTarget(struct ScreenMovingActor *actor, s32 target)
{
    s32 *speed;
    struct WorldPosition *position;
    struct ScreenPosition *screen;
    if (target - actor->screen.x < 0 || (actor->screen.x > 60 && target <= 60))
        actor->horizontalAcceleration = -128;
    else
        actor->horizontalAcceleration = 128;
    Player_GetHorizontalVelocity(actor->playerIndex);
    speed = &actor->horizontalSpeed;
    *speed += actor->horizontalAcceleration;
    if (*speed > 0x6000)
        *speed = 0x6000;
    if (*speed < 0x2000)
        *speed = 0x2000;
    position = &actor->position;
    position->x += *speed;
    actor->position.z = actor->targetZ;
    screen = &actor->screen;
    screen->x = (position->x >> 12) - ((struct DisplayCamera *)gSceneCamera)->x;
    {
        s32 y = position->y;
        s32 z = position->z;
        screen->y = ((z + y) >> 12) + 96;
    }
    screen->z = position->z >> 12;
    if (screen->x > 160) {
        position->x -= *speed;
        *speed = 0;
    }
    return 0;
}
