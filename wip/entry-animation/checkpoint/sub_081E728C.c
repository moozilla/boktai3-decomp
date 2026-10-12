#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */
struct Obj {u32 flags;u8 pad[4];s8 scaleX,scaleY;u8 padScale[6];u16 frame;u8 pad2[10];s16 x,y,z;};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct S {u8 p0[0xddc];struct Obj obj;u8 p1a[0xe08 - 0xe00];u8 effect[0xe24 - 0xe08];struct Anim anim;u8 p2[0xe74 - 0xe34];u32 arg;u16 count;s8 delta;u8 p3a[0xec8-0xe7b];s16 deltaX,deltaY,deltaZ;u8 gapCE[2];s16 targetX,targetY,targetZ;u8 gapD6[2];s16 startX,startY,startZ;u8 gapDE[2];s16 savedX,savedY,savedZ;u8 gapE6[0xf50-0xee6];void *handle;u8 p3b[4];u8 done,state;u16 mode:4;u16 phase:12;u16 oldMode;u16 speed,heading;u8 gap62[2];u8 flags;u8 gap65;u8 direction;s8 requestedDirection;u8 facing;s8 bend;u8 gap6a;u8 flip;u8 last,next;u8 p6a[0xf7c-0xf6e];u8 verticalDone;u8 gap7D[0xf86-0xf7d];u8 savedFrame,savedTimer;u8 p6b[4];void (*previous)(void *);void (*fn)(void *);void(*scaleFn)(void*);void(*otherFn)(void*);s32 timer;u32 variant;};
void sub_08220C8C(void *,u32,u32,u32,u32);
void sub_081E4188(void *);void sub_08013440(void *);void sub_081E41EC(void *);
static inline u8 take(struct S *p) {s32 m=1;register s32 v asm("r1")=p->flags;if(v&m) {p->flags=~m&v;p->done=0;return 1;}return 0;}
static inline u32 mode_bits(u8 *p,u32 mask){register u32 v asm("r1")=*p;register u32 result asm("r0")=mask&v;return result;}
static inline u32 frame_factor(u16 v) {register u32 value asm("r1")=v;u32 mask=15;return mask&value;}
struct Child {void (*fn)(void*,void*);u8 pad;u8 flags;u8 rest[0xdc-6];};void sub_081E5608(void*,void*);void sub_08056CC0(void);void sub_081E4174(void*);void sub_08215284(void*,s32);void sub_0822B2F8(s32);void sub_0821AD08(void*,s32);void sub_08013424(void*);void sub_081E34D0(void*);
extern u8 gUnk_03005430[];extern const s16 gUnk_086149C4[];u8 sub_082215E4(s32,s32);void sub_081E41B0(void*);
static inline s16 set_direction(s16 angle,u8 *directionOut,u8 *flipOut){register s32 direction asm("r2");
 while(angle<0)angle+=256;
 while(angle>255)angle-=256;
 {register s32 shifted asm("r0")=angle-16;direction=(shifted>>5)&7;}
 {register s32 choice asm("r0")=direction;
 if(choice==4)*directionOut=0;
 else if(choice==0)*directionOut=4;
 else if(choice<4){*directionOut=4-direction;*flipOut=1;}
 else{register s32 result asm("r0")=direction-4;*directionOut=result;*flipOut=0;}}return direction;
}
static inline s32 absolute(s32 v){return v<0?-v:v;}
struct Rotator {u32 flags;u8 pad[12];u16 frame;u8 pad2[12];u16 angle;u8 pad20[0x7e];s8 delta;};
static inline void rotate(struct S *p) {
 if(!(p->count++ &3)) {
 struct Rotator *o=(struct Rotator*)&p->obj;
 switch((int)(p->anim.frame&3)) {
 case 0:case 3:if(o->flags&0x10)o->delta=1;else o->delta=-8;break;
 case 1:case 2:if(o->flags&0x10)o->delta=-1;else o->delta=8;break;
 }
 o->angle+=o->delta;
 }
}

void sub_081E728C(struct S*p){
 struct Obj *o;struct Anim *a;const u16 *entry;register u16 n asm("r0");u8 done;
 s32 dx=p->targetX-p->obj.x,dz=p->targetZ-p->obj.z;
 p->heading=sub_082215E4(dx,dz);
 if(take(p)){p->state=0;{register s32 direction asm("r2")=(s16)set_direction(p->heading,&p->direction,&p->flip);register s32 facing asm("r0")=(s16)direction-4;facing&=7;p->facing=facing;}p->verticalDone=0;sub_08013424(p->p2);}
 rotate(p);
 sub_08220C8C(&p->anim,p->arg,p->state,p->direction,p->flip);
 o=&p->obj;a=&p->anim;entry=a->data+a->frame;
 o->frame=*entry>>6;
 {u32 input=a->flags;register u32 mask asm("r3")=1; register u32 left asm("r1")=(u8)(input&mask); if(left!=(((*entry&0x30)>>4)&mask))o->flags|=4;else o->flags&=~4;}
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
 if((u8)(a->flags&8)) {a->timer=a->rate;a->frame=a->count-1;done=1;}else {a->frame=0;done=1;}
 }else done=0;
 }
 entry=a->data+a->frame;a->factor=frame_factor(*entry);
 a->rate=(a->factor*a->scale)>>6;if(!a->rate)a->rate=1;
 }else done=0;
 p->done=done;
 if(absolute(dx)+absolute(dz)<=9&&p->verticalDone){
 p->obj.x=p->targetX;p->obj.z=p->targetZ;
 if(p->requestedDirection>=0){u32 flip=0;u32 direction;p->facing=p->requestedDirection;direction=p->requestedDirection&7;if(direction>4){direction=(u8)(8-direction);flip=1;}
 sub_08220C8C(&p->anim,p->arg,p->state,direction,flip);
 o=&p->obj;a=&p->anim;entry=a->data+a->frame;
 o->frame=*entry>>6;
 {u32 input=a->flags;register u32 mask asm("r3")=1; register u32 left asm("r1")=(u8)(input&mask); if(left!=(((*entry&0x30)>>4)&mask))o->flags|=4;else o->flags&=~4;}
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
 if((u8)(a->flags&8)) {a->timer=a->rate;a->frame=a->count-1;done=1;}else {a->frame=0;done=1;}
 }else done=0;
 }
 entry=a->data+a->frame;a->factor=frame_factor(*entry);
 a->rate=(a->factor*a->scale)>>6;if(!a->rate)a->rate=1;
 }else done=0;
 p->done=done;

 }
 {register u32 raw asm("r1")=*(u16*)((u8*)p+0xf5a);register u32 mask asm("r3")=15;*(u16*)((u8*)p+0xf5a)=mask&raw;
 if(p->oldMode==3){register u32 bits asm("r0")=mode_bits((u8*)p+0xf5a,mask);if(bits==3){p->savedFrame=p->anim.frame;p->savedTimer=p->anim.timer;}}}
 {register s32 mask asm("r0")=1;register s32 v asm("r1")=p->flags;p->flags=mask|v;}
 if(p->handle){void *handle=p->handle;p->handle=0;sub_0821AD08(handle,0);}
 }else{
 if(!p->verticalDone){
 if(p->targetY>p->obj.y){p->obj.y+=7;p->savedY+=7;if(p->targetY<p->obj.y){p->obj.y=p->targetY;p->savedY=p->targetY;p->verticalDone=1;}}
 else if(p->targetY<p->obj.y){p->obj.y-=7;p->savedY-=7;if(p->targetY>p->obj.y){p->obj.y=p->targetY;p->savedY=p->targetY;p->verticalDone=1;}}
 }
 if(dx||dz){p->deltaX=Div(gUnk_086149C4[(p->heading+64)&255],500);p->deltaZ=Div(gUnk_086149C4[p->heading&255],500);p->obj.x+=p->deltaX;p->obj.z+=p->deltaZ;p->savedX=p->obj.x;p->savedZ=p->obj.z;}
 }
}
