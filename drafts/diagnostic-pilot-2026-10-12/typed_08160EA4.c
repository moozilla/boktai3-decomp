#include "global.h"
struct Vec { u32 x:16; u32 y:16; u32 z:16; u32 pad:16; };
struct Data { u8 pad[16]; u16 duration,pad12,phase; u8 pad16[10]; u16 x,y,z; };
struct Obj { u8 pad[0xc0]; s32 distance; u8 padC4[0x4c0]; u32 counter,pad588; u8 *angle,*sign; struct Data *data; u32 pad598,state; };
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
s32 Mod(s32,s32);
s32 Div(s32,s32);
void sub_081606C0(struct Obj *,struct Vec *,s32,void *,s32,s32);
#define W(n) (*(u32 *)(p+(n)))
#define I(n) (*(s32 *)(p+(n)))
#define P(n) (*(u8 **)(p+(n)))
#define D(n) (*(u16 *)(P(0x594)+(n)))
static inline s32 shift12(s32 x) { if(x>=0) return x>>12; return -((-x)>>12); }
#define SIN(n) gUnk_086149C4[(n)&255]
static inline s32 shift_distance(s32 x) { if(x>=0) x=x>>12; else x=-((-x)>>12); return x; }
static inline u16 mask8(s32 x) { return x&255; }
s32 sub_08160EA4(struct Obj *p)
{
    struct Vec a,b,c;
    u32 state=p->state;
    if(state==1) {
        s32 angle,step;
        if(p->counter==0) {
            gUnk_03005308=(gUnk_03005308+1)&1023;
            p->distance=Mod(*(u16 *)((gUnk_03005308<<1)+(u32)gUnk_0203B400),10)+230;
        }
        angle=((*p->angle+5)&7)*32;
        step=Div(768,p->data->duration);
        if(p->data->phase>2) goto done;
        if(*p->sign) angle=angle-p->counter*step+288;
        else angle=p->counter*step+angle+224;
        angle=mask8(angle);
        a.x=p->data->x+shift12(p->distance*SIN(angle+64));
        a.y=p->data->y+200;
        a.z=p->data->z+shift12(p->distance*gUnk_086149C4[angle]);
        sub_081606C0(p,&a,5,(u8 *)p+0x34,0,1);
        p->counter++;
    } else if(state==2) {
        if(p->counter==0) p->distance=290;
        if(p->data->phase<=1) goto done;
        if(p->counter>9) goto done;
        if(!(p->counter&1)) {
            s32 angle,pitch,distance;
            angle=((*p->angle+5)&7)*32;
            pitch=(306-p->counter*10)&255;
            b.y=p->data->y+shift12(p->distance*SIN(pitch))+190;
            distance=shift_distance(p->distance*SIN(pitch+64));
            b.x=p->data->x+shift12(distance*SIN(angle+64))-16;
            b.z=p->data->z+shift12(distance*gUnk_086149C4[angle])+16;
            sub_081606C0(p,&b,5,(u8 *)p+0x34,0,1);
        }
        p->counter++;
    } else if(state==3) {
        if(p->counter==0) p->distance=60;
        if(!(p->counter&1)) {
            s32 angle;
            if(p->data->phase>2) p->distance-=70; else p->distance+=35;
            angle=((*p->angle+5)&7)*32;
            c.x=p->data->x+shift12(p->distance*SIN(angle+64))-16;
            c.y=p->data->y+200;
            c.z=p->data->z+shift12(p->distance*gUnk_086149C4[angle])+16;
            sub_081606C0(p,&c,5,(u8 *)p+0x34,0,1);
        }
        p->counter++;
    } else p->counter=0;
 done:
    return 0;
}
