#include "global.h"
struct Vec { u32 x:16; u32 y:16; u32 z:16; u32 pad:16; };
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
s32 Mod(s32,s32);
s32 Div(s32,s32);
void sub_081606C0(u8 *,struct Vec *,s32,void *,s32,s32);
#define W(n) (*(u32 *)(p+(n)))
#define I(n) (*(s32 *)(p+(n)))
#define P(n) (*(u8 **)(p+(n)))
#define D(n) (*(u16 *)(P(0x594)+(n)))
static inline s32 shift12(s32 x) { if(x>=0) return x>>12; return -((-x)>>12); }
#define SIN(n) gUnk_086149C4[(n)&255]
static inline s32 shift_distance(s32 x) { if(x>=0) x=x>>12; else x=-((-x)>>12); return x; }
static inline u16 mask8(s32 x) { return x&255; }
s32 sub_08160EA4(u8 *p)
{
    struct Vec a,b,c;
    u32 state=W(0x59C);
    if(state==1) {
        s32 angle,step;
        if(W(0x584)==0) {
            gUnk_03005308=(gUnk_03005308+1)&1023;
            I(0xC0)=Mod(*(u16 *)(((u16)gUnk_03005308<<1)+(u32)gUnk_0203B400),10)+230;
        }
        angle=((*P(0x58C)+5)&7)*32;
        step=Div(768,D(0x10));
        if(D(0x14)>2) goto done;
        if(*P(0x590)) angle=angle-W(0x584)*step+288;
        else angle=W(0x584)*step+angle+224;
        angle=mask8(angle);
        a.x=D(0x20)+shift12(I(0xC0)*SIN(angle+64));
        a.y=D(0x22)+200;
        a.z=D(0x24)+shift12(I(0xC0)*gUnk_086149C4[angle]);
        sub_081606C0(p,&a,5,p+0x34,0,1);
        W(0x584)++;
    } else if(state==2) {
        if(W(0x584)==0) I(0xC0)=290;
        if(D(0x14)<=1) goto done;
        if(W(0x584)>9) goto done;
        if(!(W(0x584)&1)) {
            s32 angle,pitch,distance;
            angle=((*P(0x58C)+5)&7)*32;
            pitch=(306-W(0x584)*10)&255;
            b.y=D(0x22)+shift12(I(0xC0)*SIN(pitch))+190;
            distance=shift_distance(I(0xC0)*SIN(pitch+64));
            b.x=D(0x20)+shift12(distance*SIN(angle+64))-16;
            b.z=D(0x24)+shift12(distance*gUnk_086149C4[angle])+16;
            sub_081606C0(p,&b,5,p+0x34,0,1);
        }
        W(0x584)++;
    } else if(state==3) {
        if(W(0x584)==0) I(0xC0)=60;
        if(!(W(0x584)&1)) {
            s32 angle;
            if(D(0x14)>2) I(0xC0)-=70; else I(0xC0)+=35;
            angle=((*P(0x58C)+5)&7)*32;
            c.x=D(0x20)+shift12(I(0xC0)*SIN(angle+64))-16;
            c.y=D(0x22)+200;
            c.z=D(0x24)+shift12(I(0xC0)*gUnk_086149C4[angle])+16;
            sub_081606C0(p,&c,5,p+0x34,0,1);
        }
        W(0x584)++;
    } else W(0x584)=0;
 done:
    return 0;
}
