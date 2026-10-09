// CFLAGS: -O2 -mthumb-interwork -fprologue-bugfix
#include "global.h"
extern u32 gUnk_03005308,gUnk_030053F4;
extern u16 gUnk_0203B400[];
s32 Div(s32,s32);
s32 Mod(s32,s32);
void sub_0822B358(u32);
void sub_082349A0(u8 *);
void sub_08234A84(u8 *);
void sub_08234B34(u8 *);
#define H(o) (*(u16 *)(p+(o)))
#define O(o) (*(u16 *)(*(u8 **)(p+0x18)+(o)))
static inline s32 rnd(u16 *table) { s32 v; gUnk_03005308=(gUnk_03005308+1)&1023; v=table[gUnk_03005308]; return v>>3; }
void sub_08234B98(u8 *p)
{
    s32 done,step;
    u16 *table;
    if(H(0x270)==0) { O(0x424)=0; H(0x270)++; }
    done=0;
    step=Div(O(0x426),150);
    if(step==0) step=1;
    O(0x424)+=step;
    if(O(0x424)>=O(0x426)) { O(0x424)=O(0x426); sub_0822B358(0x259); done=1; }
    table=gUnk_0203B400;
    H(0x3c)=H(0x7c)+Mod(rnd(table),3)-1;
    H(0x3e)=H(0x7e)+Mod(rnd(table),3)-1;
    if(!(H(0x26c)&7)) {
        sub_082349A0(p+0x8c+H(0x26e)*60);
        H(0x26e)++;
        if(H(0x26e)>7) H(0x26e)=0;
    }
    if(!(H(0x26c)&15)) {
        sub_08234A84(p+0x8c+H(0x26e)*60);
        H(0x26e)++;
        if(H(0x26e)>7) H(0x26e)=0;
    }
    H(0x26c)++;
    if(done) { *(void (**)(u8 *))(p+0x274)=sub_08234B34; H(0x270)=0; }
}
