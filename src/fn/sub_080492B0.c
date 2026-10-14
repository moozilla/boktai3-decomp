#include "tether_actor.h"
u32 sub_0821FCDC(void *);
void sub_0821FD3C(void *, void *);
u32 sub_0821F2EC(void *, void *, void *);
u32 TetherActor_TestInteractionPath(struct TetherActor *p)
{
    u8 *a = (u8 *)&p->position;
    u8 *b;
    u32 r;
    if (sub_0821FCDC(a) == 0xFF || (b = (u8 *)&p->interactionProbe, sub_0821FD3C(b, a),
                                    sub_0821F2EC(b, a, (u8 *)&p->interactionTarget) == 0))
        r = 0;
    else
        r = 1;
    return r;
}
