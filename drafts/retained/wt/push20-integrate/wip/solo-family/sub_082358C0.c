#include "global.h"
s32 Div(s32,s32);
void sub_0822B2F8(u32);
void sub_08235794(u8 *);
#define H(o) (*(u16 *)((u8 *)p+(o)))
#define S(o) (*(s16 *)((u8 *)p+(o)))
#define W(o) (*(u32 *)((u8 *)p+(o)))
static inline u32 clear(u32 *dst,u32 mask) { return *dst&=mask; }
struct Obj { u8 pad[0x24]; u32 flags; };
void sub_082358C0(struct Obj *p)
{
    clear(&p->flags,~1);
    H(0x34e)++;
    if(H(0x34e)>7) {
        clear(&p->flags,~2);
        *(void (**)(u8 *))((u8 *)p+0x350)=sub_08235794;
        H(0x34e)=0;
        sub_0822B2F8(0x284);
    } else {
        u32 attr=H(0x34e)*2+32;
        ((u8 *)p)[0x51]=attr; ((u8 *)p)[0x50]=attr;
        H(0x3c)=Div(S(0x7c)*H(0x34e)+S(0x84)*(8-H(0x34e)),8);
        H(0x3e)=Div(S(0x7e)*H(0x34e)+S(0x86)*(8-H(0x34e)),8);
    }
}
