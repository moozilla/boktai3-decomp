#include "global.h"
void sub_0815AD78(u8 *);
void sub_0813AFEC(u8 *,u32,u32);
void sub_0814FC24(void);
void sub_0815C230(u8 *s,s32 x,u32 y)
{
    u8 *kind;
    u8 value;
    if(x>=0) s[0x3A4]=x;
    kind=s+0x3A4;
    value=*kind;
    value|=1;
    *kind=value;
    if((u8)value>4) {
        {s32 n=(8-*kind)>>1;
        s[0x3A2]=n;}
        s[0x3A3]=1;
    } else {
        {u32 n=value>>1;
        s[0x3A2]=n;}
        s[0x3A3]=0;
    }
    *(u32 *)(s+0x5A4)=y;
    sub_0815AD78(s);
    sub_0813AFEC(s,0,0);
    *(void (**)(void))(s+0x58C)=sub_0814FC24;
}
