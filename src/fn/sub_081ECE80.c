#include "global.h"
#ifdef NONMATCHING
// fuzzy: 88.2%
// foundry draft (worker best 88.2%); what differs is mostly register allocation and instruction order
#define SceneMover_StepPastScreenX sub_081ECE80
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
extern struct SceneMoverCamera *gUnk_020004B8;
static inline void Project(struct MoverPosition *pos, struct MoverScreen *screen)
{
    s32 y;
    *(s16 *)screen = (pos->x >> 12) - gUnk_020004B8->x;
    y = pos->y;
    screen->y = ((pos->z + y) >> 12) + 96;
    screen->z = pos->z >> 12;
}
s32 sub_081ECE80(struct SceneMover *p, s32 target, s32 speed)
{
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
    Project(pos, (struct MoverScreen *)screen);
    if (delta < 0) {
        if (screen[0] < target)
            return 1;
    } else {
        if (screen[0] > target)
            return 1;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081ECE80);
#endif
