#define OrbitMotion_BeginDistanceWave sub_081B939C
#include "orbit_motion.h"
u16 Sqrt(u32);
s32 __divsi3(s32,s32);
static inline void difference(struct OrbitVector *out, struct OrbitVector *a, struct OrbitVector *b) {
    out->x=a->x-b->x; out->y=a->y-b->y; out->z=a->z-b->z;
}
static inline s32 square(s32 n) { return n*n; }
static inline s32 distance(struct OrbitVector *v) {
    return Sqrt(square(v->x)+square(v->z));
}
void sub_081B939C(struct OrbitMotion *p, struct OrbitVector *position, struct OrbitVector *end, s32 angle, s32 speed) {
    struct OrbitVector delta;
    if(p && position && end) {
        struct OrbitVector *target;
        p->position=position;
        target=&p->base;
        *target=*end;
        p->angle=angle;
        difference(&delta,target,p->position);
        { s32 length=distance(&delta); s32 factor=314*speed;
          p->radius=__divsi3(length*factor,25600); }
        if(p->radius<=1) p->radius=2;
        p->speed=speed;
        p->unk28=6;
        p->state=1;
        p->timer=0;
    }
}
