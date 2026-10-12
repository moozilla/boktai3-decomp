#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */
struct Obj {u32 flags;u8 pad[4];s8 scaleX,scaleY;u8 padScale[6];u16 frame;u8 pad2[12];u16 angle;};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct S {u8 p0[0xddc];struct Obj obj;u8 p1a[0xe08 - 0xdfc];u8 effect[0xe24 - 0xe08];struct Anim anim;u8 p2[0xe74 - 0xe34];u32 arg;u16 count;s8 delta;u8 p3a[0xf50 - 0xe7b];void *handle;u8 p3b[4];u8 done,state;u16 mode:4;u16 phase:12;u16 oldMode;u8 p4[0xf64 - 0xf5e];u8 flags;u8 p5[7];u8 last,next;u8 p6a[0xf7a-0xf6e];u16 savedY;u8 p7;u8 interrupted;u16 savedPhase;u8 p8[4];u16 delay;u8 savedFrame,savedTimer;u8 p6b[4];void (*previous)(void *);void (*fn)(void *);void (*scaleFn)(void*);void (*otherFn)(void*);s32 idleTimer;};
void sub_08220C8C(void *,u32,u32,u32,u32);
void sub_081E4188(void *);void sub_08013440(void *);void sub_081E41EC(void *);
static inline u8 take(struct S *p) {s32 m=1;register s32 v asm("r1")=p->flags;if(v&m) {p->flags=~m&v;p->done=0;return 1;}return 0;}
static inline u32 mode_bits(u8 *p,u32 mask){register u32 v asm("r1")=*p;register u32 result asm("r0")=mask&v;return result;}
static inline u32 frame_factor(u16 v) {register u32 value asm("r1")=v;u32 mask=15;return mask&value;}
void sub_081E4174(void*);void sub_08215284(void*,s32);void sub_0822B2F8(s32);void sub_0821AD08(void*,s32);void sub_08013424(void*);void sub_081E34D0(void*);
extern s32 gUnk_03005260;extern u8 gUnk_03005430[];void sub_081E43E8(void*);
void sub_081E42C4(struct S *p){
 if(gUnk_03005260){p->idleTimer=0;if(gUnk_03005430[14]==5||gUnk_03005430[14]==0||gUnk_03005430[14]==4)p->delay=1800;else p->delay=10800;}
 if(++p->idleTimer>p->delay && p->done){s32 zero;p->interrupted=0;{register u16 *source asm("r0")=(u16*)((u8*)p+0xf5a);p->savedPhase=*source>>4;}p->phase=5;
 if(p->oldMode==3 && p->mode==3){p->savedFrame=p->anim.frame;p->savedTimer=p->anim.timer;}
 {register s32 mask asm("r0")=1;register s32 v asm("r1")=p->flags;zero=0;p->flags=mask|v;}
 p->fn=sub_081E43E8;p->count=zero;p->savedY=p->obj.angle;
 }
}
