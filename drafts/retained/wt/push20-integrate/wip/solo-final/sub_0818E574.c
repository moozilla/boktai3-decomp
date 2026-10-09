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
 u8 padA8[0x38];
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
extern void (*const gUnk_0824F158[])(struct Obj *);
void sub_0818E4BC(struct Obj *);
void sub_0818E574(struct Obj *p)
{
 s32 mask=16; u8 flags=p->flags119;
 if(!(flags&mask)) {
  if(p->countdown) p->countdown--;
  gUnk_0824F158[p->state](p);
  if(p->callback) p->callback(p);
  sub_0818E4BC(p);
  if(p->callback1CC) p->callback1CC(p);
 }
}
