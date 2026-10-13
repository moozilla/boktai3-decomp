#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Actor {u8 pad[0x3a4];u8 facing;};
struct Manager {u8 pad[36];struct Actor *actor;s16 x,y,z;};
struct Controller {u16 id;u8 orientation;u8 pad3[4];u8 level;u8 pad8[8];u32 distance;u8 pad14[8];s16 x,y,z;};
extern u32 gUnk_02000560;
u32 sub_0800950C(struct Controller *);s32 sub_082215E4(s32,s32);void sub_08009500(struct Controller *,u32);
static inline s32 facingAllowed(u32 facing,s32 a,s32 b,s32 c){register u32 yes asm("r1")=1;if(a==facing||b==facing||c==facing)return yes;return 0;}
extern u16 gUnk_03005260[];
s32 sub_080096A0(struct Manager *,struct Controller *);void sub_080095B4(struct Controller *,u32);void sub_0800971C(struct Manager *,s32);
#define DirectionalController_CheckInput sub_080097E0
s32 DirectionalController_CheckInput(struct Manager *manager,struct Controller *p,s32 index){
 sub_0800950C(p);
 if(manager->actor){
  u32 mask=6;register u32 flags asm("r1");flags=gUnk_02000560;if(flags&mask)goto failed;flags&=1;if(!flags)goto failed;
  {
  s32 dx=p->x-manager->x,dz=p->z-manager->z;
  u32 distance=dx*dx+dz*dz;
  u32 direction=(((u16)sub_082215E4(dx,dz)+112)>>5)&7;
  if(distance<p->distance){
   s32 a,b,c;
   if(p->orientation){a=0;b=1;c=2;}else{a=6;b=7;c=0;}
   if(a==direction||b==direction||c==direction){
    u32 facing=manager->actor->facing;
    if(facingAllowed(facing,a,b,c)){
     if((gUnk_03005260[1]&1)&&sub_080096A0(manager,p)){
      p->level=5;sub_080095B4(p,0);sub_08009500(p,2);sub_0800971C(manager,index);
     }
     goto done;
    }
   }
  }
 }
 }
failed: sub_08009500(p,0);
done: return 0;
}
