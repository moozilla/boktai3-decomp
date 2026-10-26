#include "directional_force.h"
void DirectionalForce_ConfigureFixed(u32 a, u32 b, u32 c, u32 d, u32 e)
{
    struct DirectionalForce *p = gDirectionalForce;
    if (p) {
        u32 z = 0;
        p->mode = 2;
        p->targetAngle = a;
        p->baseMagnitude = b;
        p->magnitudeRange = c;
        p->baseDelay = d;
        p->delayRange = e;
        p->delay = z;
    }
}
