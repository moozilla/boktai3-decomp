#include "global.h"
extern u32 gUnk_0300523C;
extern const s16 gUnk_086149C4[];
s32 Div(s32,s32);
void sub_0822B410(void);
void sub_0822B2F8(u32);
void sub_08234868(u8 *);
void sub_08234D0C(u8 *);
#define H(o) (*(u16 *)(p+(o)))
#define S(o) (*(s16 *)(p+(o)))
#define W(o) (*(u32 *)(p+(o)))
static inline s32 scale(s32 x) { if(x>=0) x>>=12; else x=-((-x)>>12); return x; }
void sub_08234DC8(u8 *p)
{
    s32 age;
    if(H(0x270)==0) { u32 mask; sub_0822B410(); mask=4; gUnk_0300523C|=mask; }
    else if(H(0x270)==1) { sub_0822B2F8(230); W(0x24)&=~1; }
    age=H(0x270)+1;
    H(0x270)=age;
    if(H(0x270)>15) {
        W(0x24)&=~2;
        sub_08234868(p);
        *(void (**)(u8 *))(p+0x274)=sub_08234D0C;
        H(0x270)=0;
        sub_0822B2F8(0x284);
    } else {
        s32 y;
        p[0x51]=age+34; p[0x50]=age+34;
        H(0x3c)=Div(S(0x7c)*H(0x270)+S(0x84)*(16-H(0x270)),16);
        y=Div(S(0x7e)*H(0x270)+S(0x86)*(16-H(0x270)),16);
        H(0x3e)=y-scale(gUnk_086149C4[Div(H(0x270)*128,20)&255]*16);
    }
}
