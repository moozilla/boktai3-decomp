#include "global.h"
struct Obj { u8 pad0[0x2e]; s16 dx,dy,dz,duration; u16 from,to; u8 pad3A[2]; u32 x,y,z; };
s32 Div(s32,s32);
void sub_08179650(struct Obj *p,u32 x,u32 y,u32 z,u32 endX,u32 endY,u32 endZ,u32 duration)
{
    p->from=x|(y<<5)|(z<<10);
    p->to=endX|(endY<<5)|(endZ<<10);
    p->duration=duration;
    p->dx=Div((endX-x)<<4,p->duration);
    p->dy=Div((endY-y)<<4,p->duration);
    p->dz=Div((endZ-z)<<4,p->duration);
    p->x=x<<4; p->y=y<<4; p->z=z<<4;
}
