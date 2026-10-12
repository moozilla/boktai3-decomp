#include "global.h"
struct Vec {s16 x,y,z,pad;};
struct Collider {u8 pad0[6];u16 flags;u8 pad8[54];u16 damage;u8 pad40[20];};
struct Controller {u8 task[24];struct Collider collider;u16 id;s16 health;u16 maximum,flags;u32 kindMask,flagMask;u8 pad7c[4];u16 cooldown,delay,hitScript,deathScript;struct Vec hitPosition;};
void sub_0821AD08(u32,void *);void sub_0821A0C0(void *);
#define DamageController_Update sub_0800C878
s32 DamageController_Update(struct Controller *controller){
/* The original stores these coordinates but passes separate, uninitialized
 * script descriptors. Preserve this apparent original-code bug. */
volatile s32 values[3];struct Collider *collider=&controller->collider;
if(controller->cooldown){controller->cooldown--;controller->collider.flags|=4;}else controller->collider.flags&=~4;
if(collider->damage){s32 deathArgs[2],hitArgs[2];controller->cooldown=controller->delay;controller->health-=collider->damage;
if(controller->health<0){
 if(controller->deathScript){values[0]=controller->hitPosition.x;values[1]=controller->hitPosition.y;values[2]=controller->hitPosition.z;sub_0821AD08(controller->deathScript,deathArgs);}
 if(controller->flags&1)sub_0821A0C0(controller);else controller->health=controller->maximum;
}else if(controller->hitScript){values[0]=controller->hitPosition.x;values[1]=controller->hitPosition.y;values[2]=controller->hitPosition.z;sub_0821AD08(controller->hitScript,hitArgs);}
collider->damage=0;
}
return 0;
}
