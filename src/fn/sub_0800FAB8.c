#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {s16 x,y,z,pad;};
struct Obj {u32 flags;u8 pad[12];u16 frame;u8 pad12[10];struct Vec position;};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct Piece {u8 active,state,done,angle;u16 timer;s8 direction;u8 pad7[3];u16 targetY;u8 padc[4];struct Vec position;u8 pad18[24];struct Anim anim;struct Obj sprite;};
extern s16 gUnk_086149C4[];
static inline u32 frame_factor(u16 v){register u32 value asm("r1")=v;u32 mask=15;return mask&value;}
#define BoardPiece_UpdateAnimation sub_0800FAB8
void BoardPiece_UpdateAnimation(struct Piece *p){
 struct Obj *o=&p->sprite;struct Anim *a;const u16 *entry;register u16 n asm("r0");int done;
 p->sprite.position=p->position;
 a=&p->anim;entry=a->data+a->frame;o->frame=*entry>>6;
 {u32 input=a->flags;register u32 mask asm("r3")=1; register u32 left asm("r1")=(u8)(input&mask); if(left!=(((*entry&0x30)>>4)&mask))p->sprite.flags|=4;else p->sprite.flags&=~4;}
 {register u32 input asm("r0")=a->flags;u32 mask=2; u32 left=(u8)(mask&input); if(left!=(((*entry&0x30)>>4)&mask))o->flags|=8;else o->flags&=~8;}
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
 p->done=done;
 if(p->position.y>p->targetY)p->position.y--;else if(p->position.y<p->targetY)p->position.y++;
 {int value=gUnk_086149C4[p->angle]*32;{int delta;if(value>=0)delta=value>>12;else{int magnitude=-value;delta=-(magnitude>>12);}o->position.y=o->position.y+delta;}}
 p->angle+=4;
}
