#include "tether_actor.h"
extern u8 *gUnk_02000488;
struct P {
    u32 a, b;
};
u32 TetherActor_CopyPosition(struct P *a)
{
    struct TetherActor *p = (struct TetherActor *)gUnk_02000488;
    u32 r;
    if (p) {
        *a = *(struct P *)((u8 *)&p->position);
        r = 1;
    } else
        r = 0;
    return r;
}
