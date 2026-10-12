#include "global.h"
struct Vec {s16 x,y,z,pad;};
struct Collider {u8 pad0[12];struct Vec position;u8 pad14[32];u32 kind,flags;u16 pad3c,damage;};
struct Controller {u8 pad0[114];u16 flags;u32 kindMask,flagMask;u8 pad7c[12];struct Vec hitPosition;};
#define DamageController_AccumulateHit sub_0800C7F4
void DamageController_AccumulateHit(struct Collider *attack,struct Collider *target,struct Controller *controller){
s32 kind=0,flags;
if(controller->flags&8)kind=1;
else if(controller->flags&2){if(controller->kindMask==attack->kind)kind=1;}
else if(controller->kindMask&attack->kind)kind=1;
flags=0;
if(controller->flags&16)flags=1;
else if(controller->flags&4){if(controller->flagMask==attack->flags)flags=1;}
else if(controller->flagMask&attack->flags)flags=1;
if(kind&&flags){target->damage+=attack->damage;controller->hitPosition=attack->position;}
}
