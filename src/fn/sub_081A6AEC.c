#include "global.h"
#define Vanargand_GetTargetLateralDistance sub_081A6AEC
// confidence=low: measures separation along the projected horizontal axis only.
struct Boss { u8 pad0[0x54]; s16 position[4]; u8 pad5C[0x110-0x5C]; s16 *target; };
s32 __divsi3(s32,s32);
u16 Sqrt(u32);
static inline s32 shift(s32 x, s32 n) { s32 r; if(x>=0) r=x>>n; else r=-(-x>>n); return r; }
static inline void project(s16 *out, s16 *p) {
 s32 x=p[0],z=p[2],a,b;
 out[0]=shift((x-z)*3,5);
 a=shift((x+z)*3,5);
 b=shift(p[1]*3,5);
 out[1]=a-b; out[2]=a+b;
}
static inline void set(s16 *p,s32 x,s32 y,s32 z) { p[0]=x;p[1]=y;p[2]=z; }
static inline void unproject(s16 *out,s16 *p) {
 s32 x=p[0],y=p[1];
 out[0]=shift(__divsi3((x+y)<<5,3),1);
 out[1]=0;
 out[2]=shift(__divsi3((y-x)<<5,3),1);
}
#ifdef NONMATCHING
// fuzzy: 54.1%
// Remaining differences: temporary addresses, zero reuse, and register allocation.
u16 Vanargand_GetTargetLateralDistance(struct Boss *p) {
 s16 a[4],b[4],c[4],d[4];
 s16 *q;
 project(a,p->position);
 project(b,p->target);
 set(c,b[0]-a[0],0,0);
 unproject(d,c);
 q=d;
 return Sqrt(q[0]*q[0]+d[2]*d[2]);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081A6AEC);
#endif
