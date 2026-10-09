#include "global.h"
struct Elem { u32 flags; u8 pad4[20]; s16 coords[3]; u8 pad1E[10]; u8 state,age; u8 pad2A[2]; };
struct Owner { u8 pad[0x8c]; s16 coords[3]; u8 pad92[14]; u32 handle; struct Elem elems[8]; u8 index,timer; };
s32 Div(s32,s32);
void sub_08217EEC(struct Elem *,u32,u32);
static inline void step(s16 *dest,s16 source,s32 k,s32 divisor) { *dest=Div(*dest*k+source,divisor); }
void sub_0823350C(struct Owner *p)
{
    s32 i;
    for(i=0;i<8;i++) {
        u8 *base=(u8 *)p+i*44;
        u8 *state=base+0xcc;
        if(*state) {
            u8 *age=base+0xcd;
            u32 next=*age+1;
            u32 zero=0;
            *age=next;
            if((u8)next>5) { p->elems[i].flags|=1; *state=zero; }
            else {
                s16 *coords;
                s32 k,divisor;
                k=8-*age;
                divisor=k+1;
                coords=(s16 *)(base+0xbc);
                step(coords,p->coords[0],k,divisor); coords++;
                step(coords,p->coords[1],k,divisor); coords++;
                step(coords,p->coords[2],k,divisor);
                if(*age==4) sub_08217EEC(&p->elems[i],p->handle,9);
            }
        }
    }
}
