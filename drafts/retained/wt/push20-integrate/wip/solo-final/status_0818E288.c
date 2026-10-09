#include "global.h"
struct Vec { s32 x:16; s32 y:16; s32 z:16; };
struct Parent { u8 pad0[0x5ac]; s16 x,y,z; u8 pad5B2[2]; u8 *ptr; u16 id; u8 pad5BA[2]; u16 value,other; u8 pad5C0; u8 byte5C1,byte5C2,count,counter,angle; };
struct Obj {
 u32 flags;
 u8 pad4[0x2];
 u8 rotation;
 u8 pad7[0x1];
 s8 byte8;
 s8 byte9;
 u8 padA[0x2];
 u8 * ptrC;
 u16 mode10;
 u8 pad12[0xa];
 s16 x;
 s16 y;
 s16 z;
 u8 pad22[0x2c];
 u16 flags4E;
 u8 pad50[0x2];
 u16 param52;
 u8 pad54[0x10];
 struct Vec vec64;
 struct Vec vec6C;
 u8 pad74[0xc];
 struct Vec vec80;
 u8 pad88[0x1a];
 u16 flagsA2;
 u8 padA4[0x2];
 u16 paramA6;
 u8 padA8[0x10];
 struct Vec vecB8;
 u8 padC0[0x20];
 u16 countdown;
 u8 padE2[0x10];
 u16 velocity;
 u8 padF4[0x16];
 u16 id10A;
 u8 pad10C[0xd];
 u8 flags119;
 u8 pad11A[0x4];
 u8 byte11E;
 u8 state;
 u8 pad120[0xc];
 u8 counter12C;
 u8 byte12D;
 u8 byte12E;
 u8 pad12F[0x2d];
 struct Parent * parent;
 u8 pad160[0x6c];
 void (* callback1CC)(struct Obj *);
 void (* callback)(struct Obj *);
 u8 pad1D4[0x2];
 s16 timer;
 u8 pad1D8[0x1];
 u8 angle;
 s8 spin;
};
static inline u8 reset(u8 *p){s32 one=1;struct F {u8 flags;} *flag=(struct F *)(p+0x119);u8 old=flag->flags;if(old&one){s32 mask=~1;s32 value=mask;s32 zero;value&=old;zero=0;flag->flags=value;p[0x12e]=zero;return 1;}return 0;}
#define activate(p) reset((u8 *)(p))
u8 sub_082212DC(struct Obj *,u8,u32,u32);
void sub_08215284(u8 *,s32);
void sub_0806A160(struct Vec *,u32,struct Vec *,u32,u32,u32,u32,u32);
void sub_0822B3BC(u32);
void sub_0818E288(struct Obj *p)
{
 struct Parent *owner=p->parent;
 if(activate(p)) {
  p->flags4E|=4; p->flagsA2|=4; p->counter12C=0; p->byte12D=20; p->callback=0;
 }
 if(sub_082212DC(p,p->counter12C++,10,30)) {
  sub_08215284(p->ptrC,owner->value);
  { s32 state=5; s32 zero=0; s32 one; u8 old; s32 value; p->byte11E=zero; p->state=state; one=1; old=p->flags119; value=one; value|=old; p->flags119=value; } owner->counter--;
 }
 { u32 flags=p->counter12C&7;
  if(!flags) {
   struct Vec pos=p->vec6C,color;
   pos.x+=100; pos.y+=100; pos.z+=100;
   color.x=80; color.y=60; color.z=40;
   sub_0806A160(&pos,1,&color,0,flags,14,2,2);
   sub_0822B3BC(366);
  }
 }
}
