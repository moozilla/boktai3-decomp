#define OrbitMotion_UpdateAngle sub_081B8CB0
#define gSineTable gUnk_086149C4
#include "global.h"
extern const s16 gSineTable[];
struct OrbitVector { s16 x,y,z; };
struct OrbitMotion {
    struct OrbitVector *position;
    u8 unk04[0x18];
    struct OrbitVector center;
    s16 startAngle, endAngle;
    u8 unk28, state, angle, speed;
    s16 radius;
    u8 unk2e[4];
    u16 duration;
    s32 timer;
};
static inline s32 scale8(s32 n) { if(n>=0) return n>>8; return -((-n)>>8); }
static inline s32 scale12(s32 n) { if(n>=0) return n>>12; return -((-n)>>12); }
static inline s32 interpolate(s32 end, s32 duration, s32 start, s32 time) {
    if(time<=0) return start;
    if(time>=duration) return end;
    return start+scale8(((end-start)<<8)*time/duration);
}
static inline void polar(struct OrbitVector *v, s32 angle, s32 radius) {
    v->x=scale12(gSineTable[(angle+64)&255]*radius);
    v->y=0;
    v->z=scale12(gSineTable[angle]*radius);
}
static inline void add(struct OrbitVector *out, struct OrbitVector *a, struct OrbitVector *b) {
    out->x=b->x+a->x; out->y=b->y+a->y; out->z=b->z+a->z;
}
s32 sub_081B8CB0(struct OrbitMotion *p) {
    struct OrbitVector delta;
    p->angle=interpolate(p->endAngle,p->duration,p->startAngle,p->timer);
    polar(&delta,p->angle,p->radius);
    add(p->position,&p->center,&delta);
    if((u32)++p->timer>p->duration) { p->state=2; p->timer=0; return 1; }
    return 0;
}
