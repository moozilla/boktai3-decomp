#define PositionedEffect_UpdateOscillatingCoordinates sub_081A67A0
#define gEffectSineTable gUnk_086149C4
#include "global.h"
struct EffectVector {s16 x,y,z,pad;};
struct EffectCoordinates {u8 pad0[8];struct EffectVector position;struct EffectVector shifted;};
struct PositionedEffect {u8 pad0[0x174];struct EffectCoordinates coordinates;u8 pad18c[0x80];struct EffectVector source;u8 pad214[0x3a4];u16 angularStep,amplitudeShift;};
extern const s16 gEffectSineTable[];
void PositionedEffect_SetHalfScaleCoordinates(struct EffectCoordinates *,struct EffectVector *);
static inline s32 ShiftTowardZero(s32 x) {if(x>=0)return x>>12;return -((-x)>>12);}
static inline void SetVector(struct EffectVector *v,s32 x,s32 y,s32 z) {v->x=x;v->y=y;v->z=z;}
void sub_081A67A0(struct PositionedEffect *effect,s32 phase)
{
 struct EffectCoordinates *coordinates=&effect->coordinates;
 PositionedEffect_SetHalfScaleCoordinates(coordinates,&effect->source);
 coordinates->position.y=ShiftTowardZero(gEffectSineTable[(effect->angularStep*phase)&255]<<effect->amplitudeShift)+256;
 SetVector(&coordinates->shifted,coordinates->position.x-256,coordinates->position.y,coordinates->position.z-256);
}
