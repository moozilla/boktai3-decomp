// CFLAGS: -O2 -mthumb-interwork -fprologue-bugfix
#include "global.h"
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
s32 Mod(s32,s32);
void sub_0822B2F8(u32);
void sub_08234B98(u8 *);
#define H(o) (*(u16 *)(p+(o)))
static inline s32 rnd(u16 *table) { s32 v; gUnk_03005308=(gUnk_03005308+1)&1023; v=table[gUnk_03005308]; return v>>3; }
void sub_08234D0C(u8 *p)
{
    H(0x270)++;
    if(H(0x270)<8) {
        s32 range=8-H(0x270);
        u16 *table=gUnk_0203B400;
        H(0x3c)=H(0x7c)+Mod(rnd(table),range)-(range>>1);
        H(0x3e)=H(0x7e)+Mod(rnd(table),range)-(range>>1);
    }
    if(H(0x270)>29) { *(void (**)(u8 *))(p+0x274)=sub_08234B98; sub_0822B2F8(0x259); H(0x270)=0; }
}
