#define PositionedEffect_InitRadialAttackColliders sub_081A6284
#define gEffectSineTable gUnk_086149C4
#include "global.h"
#include "engine.h"
struct PositionedEffect { u8 pad0[0x584]; u16 attackId; u8 pad1[0x96]; u8 colliders[2][0x54]; };
struct ColliderExtent { u32 x:16,y:16,z:16; };
struct EffectVector { s16 x,y,z,pad; };
extern const s16 gEffectSineTable[];
void sub_081A6104(void);
void Collider_SetAttackProperties(void *,s32,s32,s32,s32,s32);
static inline s32 FixedProduct(s32 value) { if(value>=0) return value>>12; return -((-value)>>12); }
static inline void SetRadialOffset(struct EffectVector *v,s32 angle, s32 radius) {
 angle &= 255;
 v->x=FixedProduct(gEffectSineTable[(angle+64)&255]*radius);
 v->y=0;
 v->z=FixedProduct(gEffectSineTable[angle&255]*radius);
}
void sub_081A6284(struct PositionedEffect *actor)
{
 struct ColliderExtent size;
 struct EffectVector offset;
 u8 *collider;
 s32 angle, i;
 size.x=128; size.y=128; size.z=128;
 offset.y=0;
 collider=actor->colliders[0];
 angle=240;
 i=1;
 do {
 SetRadialOffset(&offset,angle,416);
 Collider_Init(collider,0,0x2001,0,16,&size,&offset);
 Collider_SetCallback(collider,sub_081A6104,actor);
 Collider_SetAttackProperties(collider,actor->attackId,80,0,0,30);
 collider+=0x54;
 angle+=96;
 } while(--i>=0);
}
