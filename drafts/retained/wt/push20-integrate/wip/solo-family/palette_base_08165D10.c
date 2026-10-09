#include "global.h"
struct Obj { u8 pad0[0xa18]; u16 colors[16],from,to; u8 timer,duration,active; };
extern u16 *gUnk_030042E4;
s32 Div(s32,s32);
u32 sub_08165D10(struct Obj *p)
{
    u16 *from,*to;
    s32 remaining,i;
    if(p->active==0) return 1;
    from=gUnk_030042E4+p->from*16;
    to=gUnk_030042E4+p->to*16;
    p->timer++;
    if(p->timer>=p->duration) {
        u16 *dst=p->colors;
        i=15;
        do { *dst++=*to++; } while(--i>=0);
        p->active=0;
        return 1;
    }
    remaining=p->duration-p->timer;
    for(i=0;i<16;i++) {
        if(i!=5 && i!=13) {
            u16 a=*from,b=*to;
            u32 x= a&31,y=(a>>5)&31,z=(a>>10)&31;
            u32 bx=b&31,by=(b>>5)&31,bz=(b>>10)&31;
            s32 rx=Div(x*remaining+bx*p->timer,p->duration);
            s32 ry=Div(y*remaining+by*p->timer,p->duration);
            s32 rz=Div(z*remaining+bz*p->timer,p->duration);
            p->colors[i]=rx|((u32)ry<<5)|((u32)rz<<10);
        }
        from++; to++;
    }
    return 0;
}
