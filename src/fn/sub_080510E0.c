#include "directional_force.h"
void DirectionalForce_ConfigureRandom(u32 a, u32 b, u32 c, u32 d)
{
    struct DirectionalForce *p = gDirectionalForce;
    if (p) {
        u32 z = 0;
        p->mode = 1;
        p->baseMagnitude = a;
        p->magnitudeRange = b;
        p->baseDelay = c;
        p->delayRange = d;
        p->delay = z;
    }
}
