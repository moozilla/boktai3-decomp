#include "global.h"
struct Pair { u32 a,b; };
extern u32 gUnk_030053F4;
void sub_082340D8(u8 *);
void sub_08234138(u8 *);
s32 Div(s32,s32);
void sub_0822B2F8(u32);
void sub_08233B78(u8 *);
void sub_08233B60(u8 *,void (*)(u8 *));
static inline void start(u16 *dst,u32 duration) { *dst=duration; sub_0822B2F8(duration+21); }
#define H(o) (*(u16 *)(p+(o)))
s32 sub_082342AC(u8 *p,u8 *owner,struct Pair *v,u16 mode)
{
    *(struct Pair *)(p+0x64)=*v;
    *(u8 **)(p+0x18)=owner;
    H(0x1d0)=mode;
    sub_082340D8(p);
    sub_08234138(p);
    (*(u8 **)(p+0x18))[0x4ef]++;
    if(H(0x1d0)) H(0x1d4)=120;
    else if(gUnk_030053F4&2048) H(0x1d4)=90;
    else H(0x1d4)=180;
    H(0x1d2)=Div(H(0x1d4),3);
    start((u16 *)(p+0x1d6),900);
    sub_08233B60(p,sub_08233B78);
    return 0;
}
