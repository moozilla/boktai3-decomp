#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec{s16 x,y,z,pad;};struct Sprite{u32 flags;u8 pad[12];u16 frame;u8 pad12[10];struct Vec position;u8 tail[8];};struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};struct Hazard{u8 active,state,entered,pad3;u16 duration,pad6;u32 timer;u32 hold;u8 pad10[84];struct Sprite sprite;struct Anim anim;};
struct Map{u32 pad;u8*cells;u8 pad8[28];u16 rows[1];};extern struct Map*gUnk_030052F4;extern u32 gUnk_030052F8,gUnk_030052FC;
u8*sub_0821D64C(int,int);void*sub_08012040(void);void*sub_080126DC(void);struct Hazard*sub_08012728(void*);void sub_0821FF24(void*,void*,int);void sub_0821FF50(void*,int,int);void sub_0801204C(void*,void*,int);
static inline int cellIndex(struct Vec*p){int x=p->x>>8,z=p->z>>8;if(x<0||z<0||(u32)x>=gUnk_030052F8||(u32)z>=gUnk_030052FC)return 0;return gUnk_030052F4->rows[z]+x;}
static inline u8*cell(int i){u8*p=sub_0821D64C(i,1);if(p)return p+4;return gUnk_030052F4->cells+12+i*4;}
void sub_0821FF58(void*,int,int,int,int,int);void sub_08220C8C(void*,void*,int,int,int);void sub_08215284(void*,int);
static inline u32 frame_factor(u16 v){register u32 value asm("r1")=v;u32 mask=15;return mask&value;}
#define AnimatedHazard_CreateProximity sub_08012858
int AnimatedHazard_CreateProximity(struct Vec*source,int damage,int delay,int hold){u8*m=sub_08012040();struct Hazard*p;struct Vec*pos;void*effect;u8*c;int height,slope;register int zero asm("r5");if(!m){m=sub_080126DC();if(!m)return -1;}p=sub_08012728(m);if(!p)return -1;p->sprite.position=*source;{struct Vec*ground=&p->sprite.position;int x=ground->x>>8,z=ground->z>>8,index;pos=ground;asm("" : "+r"(pos));if(x<0||z<0||(u32)x>=gUnk_030052F8||(u32)z>=gUnk_030052FC)index=0;else index=gUnk_030052F4->rows[z]+x;c=cell(index);slope=*c>>4;height=(*c&15)<<8;switch(slope){case 1:height-=(u8)ground->z;break;case 2:height-=(u8)ground->x;break;}}{register s16*h asm("r0")=&p->sprite.position.y;asm("" : : "r"(h));zero=0;asm("" : "+r"(zero));*h=height;}effect=(u8*)p+16;sub_0821FF24(effect,pos,0);{register int d asm("r9")=damage;asm("" : "+r"(d));sub_0821FF58(effect,d,50,0,8,30);}sub_0801204C(m,p,1);sub_08220C8C(&p->anim,*(void**)(m+0x34),0,0,zero);
{struct Sprite*o=&p->sprite;struct Anim*a;const u16*entry;register u16 n asm("r0");int done;
a=&p->anim;entry=a->data+a->frame;o->frame=*entry>>6;
 {u32 input=a->flags;register u32 mask asm("r3")=1; register u32 left asm("r1")=(u8)(input&mask); if(left!=(((*entry&0x30)>>4)&mask))p->sprite.flags|=4;else p->sprite.flags&=~4;}
 {register u32 input asm("r0")=a->flags;register u32 mask asm("r3")=2; register u32 left asm("r1")=(u8)(mask&input); if(left!=(((*entry&0x30)>>4)&mask))o->flags|=8;else o->flags&=~8;}
 n=++a->timer;
 if(n>=a->rate) {
 a->timer=0;
 if(a->flags&4) {
 if(a->frame==0) {
 if(a->flags&8){a->timer=a->rate;}else {a->frame=a->count-1;}done=1;
 }else {a->frame--;done=0;}
 }else {
 if(++a->frame>=a->count) {
 if((u8)(a->flags&8)) {a->timer=a->rate;a->frame=a->count-1;}else {a->frame=0;}done=1;
 }else done=0;
 }
 entry=a->data+a->frame;a->factor=frame_factor(*entry);
 a->rate=(a->factor*a->scale)>>6;if(!a->rate)a->rate=1;
 }else done=0;
}
sub_08215284(m+24,0x33F);p->timer=0;p->hold=hold;asm("" : "+r"(delay));p->duration=delay;p->active=1;return 0;}
