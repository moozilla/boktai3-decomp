#include "global.h"
extern u32 gUnk_0300523C,gUnk_030053F4;
void sub_08234868(u8 *);
void sub_0822B2F8(u32);
void sub_0822B45C(void);
void sub_0821A0C0(u8 *);
static inline void clear(u32 *dst,u32 mask) { *dst&=mask; }
#define H(o) (*(u16 *)(p+(o)))
void sub_08234B34(u8 *p)
{
    if(H(0x270)==30) { sub_08234868(p); sub_0822B2F8(0x285); *(u32 *)(p+0x24)|=1; }
    H(0x270)++;
    if(H(0x270)>59) { u32 mask,value; u32 *flags; sub_0822B45C(); flags=&gUnk_0300523C; mask=~4; clear(flags,mask); value=gUnk_030053F4; mask+=3; gUnk_030053F4=value&mask; sub_0821A0C0(p); }
}
