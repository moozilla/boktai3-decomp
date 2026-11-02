#include "vanargand_effect.h"
struct OrbitMotion;
s32 MotionTrack_Update(struct OrbitMotion *);
void Task_Kill(void *);
void VanargandEffect_UpdateLifetime(struct VanargandEffect *p)
{
    u16 *c;
    MotionTrack_Update((struct OrbitMotion *)&p->motion);
    c = &p->lifetime;
    if (--*c == 0)
        Task_Kill(p);
}
