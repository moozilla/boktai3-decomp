#include "global.h"

struct Task {
    u8 pad[0x12];
    u16 f12;
};

s32 Task_Kill(struct Task *t)
{
    t->f12 |= 1;
    return 0;
}
