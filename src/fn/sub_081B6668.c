#include "global.h"
void MotionTrack_Update(u8 *);
void Task_Kill(u8 *);
void VanargandEffect_UpdateLifetime(u8 *p)
{
    s16 *c;
    MotionTrack_Update(p + 0x60);
    c = (s16 *)(p + 0x9a);
    if (--*c == 0)
        Task_Kill(p);
}
