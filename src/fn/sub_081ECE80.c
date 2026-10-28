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
static inline void Project(struct MoverPosition *pos, struct MoverScreen *screen)
{
    s32 y;
    *(s16 *)screen = (pos->x >> 12) - ((struct SceneMoverCamera *)gSceneCamera)->x;
    y = pos->y;
    screen->y = ((pos->z + y) >> 12) + 96;
    screen->z = pos->z >> 12;
}
static inline void Project2(struct MoverPosition *pos, struct MoverScreen *screen)
{
    s32 y = pos->y;
    screen->y = ((pos->z + y) >> 12) + 96;
    screen->z = pos->z >> 12;
}
s32 SceneMover_StepPastScreenX(struct SceneMover *p0, s32 target0, s32 speed)
{
    s32 target = target0;
    struct SceneMover *p = p0;
    s32 delta = target - p->screen.x;
    struct MoverPosition *pos;
    s16 screen[3];
    s16 *out;
    if (delta < 0)
        p->speedX = -speed;
    else
        p->speedX = speed;
    pos = &p->position;
    pos->x += p->speedX;
    p->position.z = p->height;
    {
        struct MoverScreen *ms = (struct MoverScreen *)screen;
        s32 sx = pos->x >> 12;
        sx -= ((struct SceneMoverCamera *)gSceneCamera)->x;
        screen[0] = sx;
        Project2(pos, ms);
    }
    if (delta < 0) {
        if (screen[0] < target)
            return 1;
    } else {
        if (screen[0] > target)
            return 1;
    }
    return 0;
}
