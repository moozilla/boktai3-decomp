#include "global.h"
#include "background_controller.h"
struct MoverPosition {
    s32 x, y, z;
};
struct MoverScreen {
    s16 x, y, z;
};
struct SceneMover {
    u8 pad0[0x268];
    struct MoverPosition position;
    u8 pad274[4];
    struct MoverScreen screen;
    s32 speedX, accelX;
    u8 pad288[0x10];
    s32 arrived;
    u8 pad29C[0x10];
    s32 height;
    u8 pad2B0[0xF0];
    s32 speedLimit;
};
struct SceneMoverCamera {
    u8 pad[0x44];
    s32 x;
};
s32 Player_GetHorizontalVelocity(s32);
s32 SceneMover_ApproachScreenX(struct SceneMover *p, s32 target)
{
    s32 delta = target - p->screen.x;
    if ((u32)(delta + 4) <= 8) {
        if (!p->arrived)
            p->speedX = Player_GetHorizontalVelocity(p->speedLimit);
        p->accelX = 0;
        p->arrived = 1;
    } else {
        if (delta < 0) {
            if (p->speedX > 0)
                p->accelX = -512;
            else
                p->accelX = -128;
        } else {
            if (p->speedX < 0)
                p->accelX = 512;
            else
                p->accelX = 128;
        }
        p->arrived = 0;
    }
    p->speedX += p->accelX;
    {
        s32 limit = Player_GetHorizontalVelocity(p->speedLimit) + 0x2000;
        if (p->speedX > limit)
            p->speedX = limit;
        if (p->speedX < 0x2000)
            p->speedX = 0x2000;
    }
    {
        struct MoverPosition *pos = &p->position;
        struct MoverScreen *screen;
        pos->x += p->speedX;
        p->position.z = p->height;
        screen = &p->screen;
        screen->x = (pos->x >> 12) - ((struct SceneMoverCamera *)gSceneCamera)->x;
        {
            s32 y = pos->y;
            screen->y = ((pos->z + y) >> 12) + 96;
        }
        screen->z = pos->z >> 12;
        if (screen->x > 160) {
            pos->x -= p->speedX;
            p->speedX = 0;
        }
    }
    return 0;
}
