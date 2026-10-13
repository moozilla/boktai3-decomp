#include "global.h"
// MATCHING_CONCESSION: register-pinning
// Tentative: creates a pooled interactable with route, target, or explicit placement.
struct Vec {s16 x,y,z,pad;};
struct Resource {u32 data[8];};
struct Route {u32 data[2];void *current;};
struct Drawable {u32 pad[2],flags;u8 rest[20];struct Vec position;u8 tail[56];};
struct Slot {u8 active,motion,state,flags,entered,flash;s16 offset,timeout;u16 delay;s32 counter;u16 id,script;s32 region;u32 kind,arg;struct Vec *target;u16 speed,range;struct Route route;struct Drawable drawable;u8 collider[84];struct Resource *resource;};
struct Manager {u8 pad[24];u32 count;struct Slot *slots;struct Resource resources[3];};
extern struct Manager *gUnk_02000034;extern u16 gUnk_086049CE[][2],gUnk_086049DA[];
struct Slot *sub_08008A70(struct Manager *);s32 Script_SeekToKeyword(u32);s32 Script_GetValue(void);s32 sub_0821ABA8(u32,s32);
u8 sub_0821E5C4(struct Route *,s32,s32,s32);u16 sub_0821E7E4(struct Route *);void sub_0821E780(struct Vec *,void *,u32);struct Vec *sub_08103EF4(s32);void sub_08008A98(struct Slot *,s32);
s32 sub_0822C034(struct Vec *);void sub_08220D78(void *,void *,u32,u32,u32);void sub_08219A80(void *,u32);void sub_08214424(void *,u32);
struct Map {u32 pad0;u8 *cells;u8 pad8[28];u16 rows[1];};
extern struct Map *gUnk_030052F4;extern u32 gUnk_030052F8,gUnk_030052FC;
u8 *sub_0821D64C(s32,s32);
static inline s32 cellIndex(struct Vec *pos){s32 x=pos->x>>8,z=pos->z>>8;if(x<0||z<0||(u32)x>=gUnk_030052F8||(u32)z>=gUnk_030052FC)return 0;return gUnk_030052F4->rows[z]+x;}
static inline s32 groundHeight(struct Vec *position){
 s32 index=cellIndex(position),height;u8 *cell=sub_0821D64C(index,1);u32 kind;
 if(cell)cell+=4;else cell=gUnk_030052F4->cells+12+index*4;
 kind=*cell>>4;height=(*cell&15)<<8;
 switch(kind){case 1:height-=(u8)position->z;break;case 2:height-=(u8)position->x;break;}
 return height;
}

#define Breakable_ScriptCreate sub_080091C8
s32 Breakable_ScriptCreate(void){
 struct Vec position;struct Manager *manager=gUnk_02000034;struct Slot *slot;s32 placed;void *drawable;
 if(!manager)goto fail;asm("" : : "r"(manager));
 slot=sub_08008A70(manager);if(!slot)goto fail;
 slot->motion=0;placed=0;
 if(Script_SeekToKeyword('r')){
  s32 route=Script_GetValue(),point=Script_GetValue(),mode=Script_GetValue();
  if(!sub_0821E5C4(&slot->route,route,point,mode))goto fail;
  slot->motion=1;slot->delay=sub_0821E7E4(&slot->route);
  slot->speed=Script_GetValue();slot->range=Script_GetValue();
  sub_0821E780(&position,slot->route.current,0);placed=1;
 }
 if(Script_SeekToKeyword('t')){
  s32 target=Script_GetValue();slot->offset=Script_GetValue();
  slot->target=sub_08103EF4(target);if(!slot->target)goto fail;
  slot->motion=2;position=*slot->target;placed=1;
 }
 if(Script_SeekToKeyword('P')){position.x=Script_GetValue();position.y=Script_GetValue();position.z=Script_GetValue();placed=1;}
 if(!placed){fail:return -1;}
 sub_08008A98(slot,sub_0821ABA8('m',1));
 slot->flags=sub_0821ABA8('f',0);slot->timeout=sub_0821ABA8('d',120);
 slot->id=sub_0821ABA8('n',0);slot->kind=sub_0821ABA8('D',0);
 slot->resource=&manager->resources[slot->kind];
 slot->script=sub_0821ABA8('p',0);slot->arg=sub_0821ABA8('a',0);
 {u32 motion=slot->motion;register struct Resource **resource asm("r7")=&slot->resource;
 if(motion!=2)position.y=groundHeight(&position);
 if(sub_0821ABA8('M',0))slot->region=sub_0822C034(&position);else slot->region=-1;
 slot->drawable.position=position;slot->drawable.flags&=~1;
 if(slot->state==1){
  drawable=&slot->drawable;
  {struct Resource *res=*resource;sub_08220D78(drawable,res,gUnk_086049CE[slot->kind][0],1,0);}
  if(slot->kind!=2)sub_08219A80(drawable,gUnk_086049DA[slot->kind]+1);
  else sub_08219A80(drawable,gUnk_086049DA[2]);
 }else{
  drawable=&slot->drawable;
  {struct Resource *res=*resource;sub_08220D78(drawable,res,gUnk_086049CE[slot->kind][1],1,0);}
  sub_08219A80(drawable,gUnk_086049DA[slot->kind]);
 }
 sub_08214424(drawable,0);slot->active=1;return 0;}
}
