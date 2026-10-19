#define PositionedEffect_InitAttackCollider sub_081A61EC
#include "global.h"
#include "engine.h"
struct PositionedEffect { u8 pad0[0x54]; u8 position[8]; u8 pad1[0x528]; u16 attackId; u8 pad2[0x42]; u8 collider[0x54]; };
struct ContactVector { u32 x:16,y:16,z:16; };
void sub_081A6104(void);
void Collider_SetAttackProperties(void *, s32,s32,s32,s32,s32);
void sub_081A61EC(struct PositionedEffect *actor)
{
 struct ContactVector size,offset;
 void *collider=actor->collider;
 size.x=128; size.y=256; size.z=128;
 *(u32 *)&offset=0x800000;
 ((u32 *)&offset)[1]&=0xffff0000;
 Collider_Init(collider,0,0x2001,0,16,&size,&offset);
 Collider_SetPosition(collider,actor->position,0);
 Collider_SetCallback(collider,sub_081A6104,actor);
 Collider_SetAttackProperties(collider,actor->attackId,80,0,0,80);
}
