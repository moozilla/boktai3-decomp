#define PositionedEffect_SpawnScatteredParticles sub_081A6398
#define gEffectRandomIndex gUnk_03005308
#define gEffectRandomTable gUnk_0203B400
#include "global.h"
struct ParticleVector { u32 x:16,y:16,z:16; };
struct PositionedEffect {u8 pad0[0x54];struct ParticleVector position;};
extern u32 gEffectRandomIndex;
extern const u16 gEffectRandomTable[];
s32 SceneParticle_CreateParticles(const void *,const void *,const void *,const void *,s32,u32);
static inline u32 RandomValue(void) {gEffectRandomIndex=(gEffectRandomIndex+1)&1023;return *(u16 *)((gEffectRandomIndex<<1)+RANDOM_TABLE_ADDR);}
static inline void ClearVector(void *p) { u32 zero=0; CpuSet(&zero,p,0x05000002); }
void sub_081A6398(struct PositionedEffect *actor)
{
 struct ParticleVector base,offset;
 s32 i;
 struct ParticleVector *b;
 void *anchor=(u8 *)actor+0x48;
 ClearVector(&base);
 b=&base;
 i=3;
 do {
 offset.x=(RandomValue()&255)+b->x-128;
 offset.y=b->y;
 offset.z=(RandomValue()&255)+b->z-128;
 SceneParticle_CreateParticles((u8 *)anchor+12,&offset,0,0,1,1);
 }while(--i>=0);
}
