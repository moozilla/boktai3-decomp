#include "global.h"
struct Vec { s16 x,y,z,pad; };
struct Obj { u8 pad0[0x40]; u32 flags; u8 pad44[0x14]; struct Vec pos; u8 pad60[0x3a]; u16 active,enable,hidden,timer,duration; struct Vec current,from,to; };
extern u8 *gUnk_02000710;
s32 Div(s32,s32);
void sub_08055828(struct Obj *p)
{
    if(p->active) {
        p->timer++;
        if(p->timer>=p->duration) { p->current=p->to; p->active=0; }
        else {
            s32 remaining=p->duration-p->timer;
            p->current.x=Div(p->to.x*p->timer+p->from.x*remaining,p->duration);
            p->current.y=Div(p->to.y*p->timer+p->from.y*remaining,p->duration);
            p->current.z=Div(p->to.z*p->timer+p->from.z*remaining,p->duration);
        }
        p->pos=p->current;
    }
    if(p->enable) {
        if(p->hidden==0 && *(s16 *)(gUnk_02000710+0x14)!=0) p->flags&=~1;
        else p->flags|=1;
    }
}
