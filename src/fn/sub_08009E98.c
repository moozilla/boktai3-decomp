#include "global.h"
struct Vec {u32 x:16,y:16,z:16;};
struct Slot {u8 pad0[5];u8 active;u8 pad6[14];u8 collider[84];u8 sprite[56];};
struct Manager {u8 pad[28];void *graphics;};
void sub_0821FEB4(void *,u32,u32,u32,u32,struct Vec *,struct Vec *);void sub_0821FF84(void *,void *,void *);void sub_0821FE40(void *);void sub_08009E5C(void);
void sub_08217DE0(void *,void *,u32);void sub_08217EEC(void *,void *,u32);void sub_08217EC4(void *,s32,s32);
#define CollisionController_InitSlot sub_08009E98
s32 CollisionController_InitSlot(struct Manager *manager,struct Slot *slot){
 struct Vec scale,offset;void *collider,*sprite;
 scale.x=30;scale.y=320;scale.z=30;
 offset.x=0;offset.y=160;offset.z=0;
 collider=slot->collider;
 sub_0821FEB4(collider,0,0x4020,0,16,&scale,&offset);
 sub_0821FF84(collider,sub_08009E5C,slot);sub_0821FE40(collider);
 sprite=slot->sprite;
 sub_08217DE0(sprite,manager->graphics,1);sub_08217EEC(sprite,manager->graphics,12);sub_08217EC4(sprite,-8,-8);
 slot->active=1;
}
