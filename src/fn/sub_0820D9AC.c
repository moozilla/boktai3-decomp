#include "global.h"
struct O {
    u8 p[0x20];
    u32 f4c;
    u32 f50;
};
void EnemyParticle_SetVerticalMotion(struct O *o, u32 a, u32 b)
{
    o->f4c = a;
    o->f50 = b;
}
