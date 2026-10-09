#include "global.h"
struct Elem { u32 flags; u8 pad4[20]; s16 coords[3]; u8 pad1E[10]; u8 state,age; u8 pad2A[2]; };
struct Owner { u8 pad[0x8c]; s16 coords[3]; u8 pad92[14]; u32 handle; struct Elem elems[8]; u8 index,timer; };
s32 Div(s32,s32);
void sub_08217EEC(struct Elem *,u32,u32);
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
                s32 k=8-*age;
                u32 divisor=k+1;
                u8 *coords=base+0xbc;
                *(s16 *)coords=Div(*(s16 *)coords*k+p->coords[0],divisor); coords+=2;
                *(s16 *)coords=Div(*(s16 *)coords*k+p->coords[1],divisor); coords+=2;
                *(s16 *)coords=Div(*(s16 *)coords*k+p->coords[2],divisor);
                if(*age==4) sub_08217EEC(&p->elems[i],p->handle,9);
            }
        }
    }
}
