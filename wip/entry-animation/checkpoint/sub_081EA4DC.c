#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */
struct Obj {u32 flags;u8 pad[3];u8 priority;s8 scaleX,scaleY;u8 padScale[6];u16 frame;u8 pad2[10];s16 x,y,z;};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct Child {u32 flags;u8 rest[0xa8];void (*update)(void*);u8 index,control;u8 gap[6];s32 count;u32 scenes[8];};
struct Asset {u32 a,b,c;u16 *palette;u8 rest[12];};
struct S {u8 head[0x1c];struct Child children[16];struct Obj obj;u8 p1a[0xe08 - 0xe00];struct Asset effect;struct Anim anim;u8 p2[0xe74 - 0xe34];u32 arg;u16 count;s8 delta;u8 p3a[0xec8-0xe7b];s16 deltaX,deltaY;u8 gapCC[4];s16 targetX,targetY,targetZ;u8 gapD6[2];s16 startX,startY,startZ;u8 gapDE[10];u16 paletteAlt[16],paletteCurrent[16],paletteOriginal[16];u16 *palette;u8 gap4C[4];void *handle;u8 p3b[4];u8 done,state;u16 mode:4;u16 phase:12;u16 oldMode;u16 speed,heading;u8 gap62[2];u8 flags;u8 gap65;u8 direction;u8 gap67[2];s8 bend;u8 gap6a;u8 flip;u8 last,next;u8 p6a[0xf7a-0xf6e];u16 savedY;u8 gap7C[0xf86-0xf7c];u8 savedFrame,savedTimer;u8 p6b[2];u16 resource;void (*previous)(void *);void (*fn)(void *);void(*scaleFn)(void*);void(*otherFn)(void*);s32 timer;u32 variant;};
void sub_08220C8C(void *,u32,u32,u32,u32);
void sub_081E4188(void *);void sub_08013440(void *);void sub_081E41EC(void *);
extern struct S *gUnk_020003C8;extern u32 gUnk_02000580;
void *sub_0821A520(u32,u32);void sub_082151E4(void*,u32);void sub_082144A4(void*,void*,u32);void sub_08214584(void*,s32,s32);void sub_080135E8(void*,void*,u32);void sub_08013440(void*);void sub_081E5604(void*);s32 sub_08225984(u32);void sub_08225AB4(void*,u32,void*,u32,u32,void*);
static inline u16 color(u16 input,u32 mask){register u32 value asm("r1")=input;return mask&value;}
s32 sub_081EA4DC(struct S*p){struct Obj *obj;struct Asset *asset;u16 *palette;s32 i;
 gUnk_020003C8=p;*(u32*)((u8*)p+0x18)=gUnk_02000580;
 obj=&p->obj;p->arg=(u32)sub_0821A520(0x922e,0xc6a0);
 asset=&p->effect;sub_082151E4(asset,0x97d3);sub_082144A4(obj,asset,0);sub_08214584(obj,-8,-19);
 i=0;obj->priority=i;obj->flags=3;
 sub_080135E8(p->p2,&p->obj.x,0);sub_08013440(p->p2);
 p->delta=1;p->count=i;*(u16*)((u8*)p+0xe7c)=i;
 p->palette=p->effect.palette;palette=p->palette;
 {u16 *src=palette,*dst=p->paletteCurrent;i=15;do{dst[16]=*src;*dst=color(*src,0xffff);src++;dst++;}while(--i>=0);}
 {u16 *src=palette+16,*dst=p->paletteAlt;i=15;do{*dst++=*src++;}while(--i>=0);}
 p->direction=0;p->heading=0;p->flip=0;
 for(i=0;i<16;i++){struct Child *child=&p->children[i];struct Asset *asset=(struct Asset*)((u8*)child+0x2c);sub_082151E4(asset,0xa850);sub_082144A4(child,asset,0);sub_08214584(child,-8,-16);
 child->flags=1;child->index=i;child->control=2;child->update=sub_081E5604;child->control|=1;}
 p->scaleFn=0;p->otherFn=0;p->obj.x=0;p->obj.y=0;p->obj.z=0;
 *(u16*)((u8*)p+0xecc)=0;p->deltaY=0;p->deltaX=0;p->savedY=0;
 p->targetX=25;p->targetY=102;p->targetZ=0;p->speed=200;
 if(!sub_08225984(p->resource))sub_08225AB4((u8*)p+0xe80,p->resource,(u8*)p+0xee0,0,7,p);
 p->variant=0;return 0;
}
