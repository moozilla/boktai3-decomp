#include "global.h"
void sub_0822B2F8(u32);
void sub_08171410(u8 *,u32);
void sub_08171EFC(u8 *s)
{
    u16 *timer=(u16 *)(s+0xA4E);
    u16 value;
    if(!*timer) sub_0822B2F8(0x10E);
    value=*timer+1;
    *timer=value;
    if((u16)value<=11) {
        if((value>>1)&1) *(u16 *)(s+0x26A2)=104;
        else {
            s32 n=(20-*timer)>>3;
            *(u16 *)(s+0x26A2)=104-n;
        }
    } else if((u16)value<=15) {
        *(u16 *)(s+0x26A2)=104;
    } else sub_08171410(s,0);
}
