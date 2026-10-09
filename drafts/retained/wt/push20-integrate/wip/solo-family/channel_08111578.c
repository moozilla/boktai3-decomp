#include "global.h"
static inline u32 channel(u16 value,u32 shift) { return (value>>shift)&31; }
struct Obj { u8 pad0[0x24]; u16 *source; s32 rx[16][16],ry[16][16],rz[16][16],bx[16][16],by[16][16],bz[16][16]; u32 end,pad182C,duration,index,target; s32 remaining; u32 flags; };
extern u16 gUnk_03004FC0[];
s32 Div(s32,s32);
void sub_08111578(struct Obj *p)
{
    u16 *base=gUnk_03004FC0+p->index*16;
    u16 *target=p->source+p->target*16;
    s32 i;
    p->end=p->duration;
    for(i=0;i<16;i++) {
        u16 a=target[i],b=*base;
        p->rx[p->index][i]=((u32)channel(a,0)-channel(b,0))<<24;
        p->ry[p->index][i]=((u32)(channel(a,5))-(channel(b,5)))<<24;
        p->rz[p->index][i]=((u32)(channel(a,10))-(channel(b,10)))<<24;
        p->rx[p->index][i]=Div(p->rx[p->index][i],p->duration);
        p->ry[p->index][i]=Div(p->ry[p->index][i],p->duration);
        p->rz[p->index][i]=Div(p->rz[p->index][i],p->duration);
        b=*base;
        p->bx[p->index][i]=(u32)channel(b,0)<<24;
        p->by[p->index][i]=(u32)(channel(b,5))<<24;
        p->bz[p->index][i]=(u32)(channel(b,10))<<24;
        base++;
    }
    if(p->remaining>=0) { p->remaining--; p->target++; p->flags|=1; }
}
