#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {s16 x,y,z,pad;};
struct Actor {u8 pad[48];struct Vec position;};
struct Controller {u16 id;u8 orientation,flags,disabled,state,entered,level;s32 region;u16 startScript,endScript;u32 distance;s16 arguments[4];struct Vec position;u32 drawableFlags;u8 rest[68];};
struct Manager {u8 pad[24];u32 count:8;s32 selected:8;s32 pending:8;u32 unused:8;u32 enabled;struct Controller *slots;struct Actor *actor;struct Vec position;};

struct PackedVec {s32 x:16,y:16,z:16,pad:16;};
extern struct Manager *gUnk_02000038;
s32 sub_0821ABA8(u32,s32);s32 Script_SeekToKeyword(u32);s32 Script_GetPc(void);s32 Script_GetValue(void);
void sub_080095D4(struct Manager *,struct Controller *,struct PackedVec *);s32 sub_0822C034(struct Vec *);void sub_080095B4(struct Controller *,u32);
#define DirectionalController_ScriptCreate sub_08009B7C
s32 DirectionalController_ScriptCreate(void){
 struct PackedVec position;s32 findRegion;
 struct Manager *manager=gUnk_02000038;struct Controller *p;s32 i,id;
 if(!manager)goto fail;
 id=sub_0821ABA8('n',0);p=manager->slots;
 for(i=0;i<manager->count;i++,p++){
  if(!(manager->enabled&(1<<i))){
   u32 *drawable=&p->drawableFlags;
   p->id=id;p->orientation=sub_0821ABA8('d',0);p->flags=0;
   p->disabled=sub_0821ABA8('t',0);findRegion=sub_0821ABA8('m',0);
   p->startScript=sub_0821ABA8('b',0);p->endScript=sub_0821ABA8('e',0);
   {s32 distance=sub_0821ABA8('D',230);p->distance=distance*distance;}
   if(Script_SeekToKeyword('a')){
    register s16 *base asm("r6")=p->arguments;s16 *out=base;
    do{s32 value=Script_GetPc();if(value)value=Script_GetValue();*out++=value;}while((s32)out<=(s32)(base+3));
   }else{
    s16 *base=p->arguments;s32 zero=0;s16 *out=p->arguments+3;do{*out--=zero;}while((s32)out>=(s32)base);
   }
   if(Script_SeekToKeyword('p')){position.x=Script_GetValue();position.y=Script_GetValue();position.z=Script_GetValue();}
   else{position.x=0;position.y=0;position.z=0;}
   sub_080095D4(manager,p,&position);
   if(findRegion)p->region=sub_0822C034(&p->position);else p->region=-1;
   sub_080095B4(p,2);
   if(p->disabled==1)*drawable|=1;else *drawable&=~1;
   if(p->orientation==0)*drawable&=~4;else *drawable|=4;
   manager->enabled|=1<<i;break;
  }
 }
 if(i!=manager->count)return 0;
 fail:return -1;
}
