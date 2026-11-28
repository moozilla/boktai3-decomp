#include "global.h"
#include "bosses/ratatosk/ratatosk_small_particle.h"

s32 sub_081FB80C(struct Particle *e)
{
    u32 bit;
    e->fallSpeed += e->config.scale;
    e->fallDistance += e->fallSpeed;
    bit = 1;
    e->motionFlags |= bit;
    return 0;
}
