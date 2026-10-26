#include "global.h"
#include "ram.h"

extern u8 *gGameState;
extern u16 gUnk_02000528, gUnk_02000590;

struct CueTarget {
    u8 pad0[0x428];
    u16 f428;
};
struct CueTask {
    u8 pad0[0x18];
    u8 side;
    u8 started;
    u8 pad1A[2];
    struct CueTarget *target;
};

void sub_08235DB0(struct CueTask *p)
{
    p->target->f428 = 0;
    if (p->side == 0) {
        *(u16 *)(gGameState + 0x2C) = 0;
        {
            u16 one = 1;
            u16 time = 0x708;
            gUnk_02000528 = one;
            gPlayerCueTimer = time;
        }
        gUnk_02000590++;
        p->started = 1;
    } else {
        *(u16 *)(gGameState + 0x3A) = 0;
        {
            u16 two = 2;
            u16 time = 0x708;
            gUnk_02000528 = two;
            gPlayerCueTimer = time;
        }
    }
}
