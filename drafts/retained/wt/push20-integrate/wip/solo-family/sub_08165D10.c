#include "global.h"
union Color { u16 value; struct { u16 r:5,g:5,b:5; } bits; };
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
            union Color a,b;
            u32 x,y,z;
            u32 bx,by,bz;
            s32 rx,ry,rz;
            a.value=*from;
            x=a.bits.r; y=a.bits.g; z=a.bits.b;
            b.value=*to;
            bx=b.bits.r; by=b.bits.g; bz=b.bits.b;
            rx=Div(x*remaining+bx*p->timer,p->duration);
            ry=Div(y*remaining+by*p->timer,p->duration);
            rz=Div(z*remaining+bz*p->timer,p->duration);
            p->colors[i]=((u32)rz<<10)|((u32)ry<<5)|rx;
        }
        from++; to++;
    }
    return 0;
}
