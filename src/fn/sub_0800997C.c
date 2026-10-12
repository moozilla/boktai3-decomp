#include "global.h"
struct Vec {s16 x,y,z,pad;};
struct Actor {u8 pad[48];struct Vec position;};
struct Controller {u16 id;u8 orientation,flags,disabled,state,entered,level;s32 region;u16 startScript,endScript;u32 distance;s16 arguments[4];struct Vec position;u32 drawableFlags;u8 rest[68];};
struct Manager {u8 pad[24];u32 count:8;s32 selected:8;s32 pending:8;u32 unused:8;u32 enabled;struct Controller *slots;struct Actor *actor;struct Vec position;};
struct World {u8 pad[0x5a0];s32 region;};
extern struct World *gUnk_02000710;extern void *gUnk_086049F4[];
void sub_08249244(struct Manager *,struct Controller *,s32,void *);void sub_0800962C(struct Manager *,struct Controller *);void sub_080096D4(struct Manager *,struct Controller *,u32);
static inline u8 sameRegion(s32 region){if(gUnk_02000710->region==region)return 1;return 0;}
#define DirectionalController_UpdateAll sub_0800997C
s32 DirectionalController_UpdateAll(struct Manager *manager){
 struct Controller *p;s32 i;
 if(manager->actor)manager->position=manager->actor->position;
 p=manager->slots;
 for(i=0;i<manager->count;i++,p++){
  if(manager->enabled&(1<<i)){
   if(p->region>=0&&!sameRegion(p->region)){
    if(p->disabled!=1)p->drawableFlags|=1;
   }else{
    if(p->disabled!=1)p->drawableFlags&=~1;
    sub_08249244(manager,p,i,gUnk_086049F4[p->state]);
    sub_0800962C(manager,p);
   }
  }
 }
 if(manager->pending>=0&&manager->selected>=0){
  p=&manager->slots[manager->selected];
  if(manager->pending==0){manager->pending=-1;sub_080096D4(manager,p,p->startScript);}
  else if(manager->pending==1){manager->pending=-1;manager->selected=-1;sub_080096D4(manager,p,p->endScript);}
 }
 return 0;
}
