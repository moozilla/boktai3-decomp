#include "global.h"
struct Vec {s32 x:16,y:16,z:16,pad:16;};
struct Attack {u8 pad0[56];u32 flags;u16 pad3C,damage;};
struct Object {u8 pad0[36];s16 health;u16 maximum,cooldown,phase,timer,flash;u8 pad30[6];u16 count,kind;u8 pad3A[58];struct Vec position;};
void sub_08215284(void *,u32);s32 sub_0800AFE8(s32,u32,s32,struct Vec *,void *,s32,s32);
#define EffectEmitter_ReceiveHit sub_0800C49C
void EffectEmitter_ReceiveHit(struct Attack *attack,void *unused,struct Object *object){
s32 damage=attack->damage;struct Vec position;
if(!damage){u32 mask=64;if(attack->flags&mask)damage=50;else damage=30;}
if(!object->cooldown&&damage>0){
 object->cooldown=8;sub_08215284((u8 *)object+0x3c,487);object->flash=8;
 object->health-=damage;
 if(object->health<=0){object->health=object->maximum;position=object->position;position.y+=64;sub_0800AFE8(object->count,1,object->kind,&position,0,600,100);}
}
}
