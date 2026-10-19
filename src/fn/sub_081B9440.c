#define OrbitMotion_BeginArc sub_081B9440
#define OrbitMotion_CalculateArcCenter sub_081B90F0
#include "orbit_motion.h"
s32 OrbitMotion_CalculateArcCenter(struct OrbitVector *, struct OrbitVector *, struct OrbitVector *, s32);
s32 Math_Atan2(s32,s32);
void sub_081B9440(struct OrbitMotion *p, struct OrbitVector *position, struct OrbitVector *end, s32 sweep, s32 duration) {
    if(p && position && end) {
        struct OrbitVector *target;
        s32 angle;
        p->position=position;
        target=&p->base;
        *target=*end;
        p->radius=OrbitMotion_CalculateArcCenter(&p->center,position,target,sweep);
        { s32 cx=p->center.x; s32 cz=p->center.z;
          struct OrbitVector *current=p->position;
          s32 x=current->x; s32 z=current->z;
          angle=Math_Atan2(x-cx,z-cz); }
        p->startAngle=angle;
        p->endAngle=angle+sweep;
        p->duration=duration;
        p->unk28=8;
        p->state=1;
        p->timer=0;
    }
}
