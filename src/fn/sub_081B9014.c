#define OrbitMotion_InterpolatePosition sub_081B9014
#include "orbit_motion.h"
s32 sub_081B9014(struct OrbitMotion *p) {
    struct OrbitVector *out=p->position;
    struct OrbitVector *start=&p->start;
    struct OrbitVector *end=&p->base;
    if((u32)p->timer>=p->duration) {
        *out=*end;
        p->state=2;
        return 1;
    }
    out->x=interpolate(end->x,p->duration,start->x,p->timer);
    out->y=interpolate(end->y,p->duration,start->y,p->timer);
    out->z=interpolate(end->z,p->duration,start->z,p->timer);
    ++p->timer;
    return 0;
}
