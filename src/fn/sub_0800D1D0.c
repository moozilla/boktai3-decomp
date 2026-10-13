#include "global.h"
struct Vec {s16 x,y,z,pad;};
struct Sprite {u32 flags;u8 pad4[12];u16 frame;u8 pad12[10];struct Vec position;u8 tail[8];};
struct Object {u16 id;u8 type,entered,state,flags;u16 timer,parameter;u8 padA,effectActive,animated,padD,frameTimer,frame;s16 health;u16 speed,ground,pad16;struct Vec position;void *collider,*attack;u8 resource[28];struct Sprite sprite;u8 anim[16];};
struct Manager {u8 task[24];void *animation;};extern struct Manager *gUnk_0200046C;
void sub_08220C8C(void *,void *,u32,u32,u32);void sub_0822B2F8(u32);void sub_0821FF24(void *,void *,u32);void sub_0821FE40(void *);
static inline void setFrame(struct Sprite *sprite,u16 frame){sprite->frame=frame;}
void sub_0800CC34(void *);void sub_08015F60(u32,void *,u32,u32,u32,u32,u32,u32,u32,u32);extern u32 gUnk_03005308;extern u16 gUnk_0203B400[];
static inline u32 randomValue(void){u32 offset;gUnk_03005308=(gUnk_03005308+1)&1023;offset=gUnk_03005308*2;return *(u16 *)(offset+(u32)gUnk_0203B400);}
static inline void setState(struct Object *object,u32 state){u32 value=state;u32 zero=0;object->state=value;object->timer=zero;object->entered=1;}
#define FallingObject_UpdateLanding sub_0800D1D0
void FallingObject_UpdateLanding(struct Object *object){
if(object->entered){object->entered=0;sub_08220C8C(object->anim,gUnk_0200046C->animation,2,0,0);sub_0800CC34(object);sub_08015F60((randomValue()&3)+6,&object->position,9,20,8,8,0,256,24,16);}
if(object->type>1){object->frameTimer++;if(object->frameTimer==10){object->frame++;if(object->frame>2)object->frame=0;object->frameTimer=0;}setFrame(&object->sprite,object->frame);}
if(object->health<=0){setState(object,3);return;}
if(object->timer++>30){setState(object,2);}
else {s32 amount=(30-object->timer)>>1;if(!(object->timer&1)){object->sprite.position.x+=amount;object->sprite.position.z-=amount;}else{object->sprite.position.x-=amount;object->sprite.position.z+=amount;}}
if(object->collider)sub_0821FF24(object->collider,&object->position,0);
if(object->attack){sub_0821FF24(object->attack,&object->position,0);sub_0821FE40(object->attack);}
}
