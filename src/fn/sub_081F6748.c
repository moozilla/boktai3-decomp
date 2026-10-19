
#ifdef NONMATCHING
// fuzzy: 99.4%
// foundry draft (worker best 99.4%); what differs is mostly register allocation and instruction order
#define SceneActors_FireTrackingProjectileSpread sub_081F6748
#define Projectile_SpawnAtPosition sub_081FD01C
#define gSceneAngleSineTable gUnk_086149C4
#include "global.h"
typedef struct ProjectilePosition { s16 x,y,z,pad; } ProjectilePosition;
typedef struct SpreadProjectileActor {
 u8 pad0[0xa8]; u8 facing; u8 padA9[0xe67]; ProjectilePosition target;
} SpreadProjectileActor;
extern s16 gUnk_086149C4[];
s32 Math_Atan2(s32,s32);
void Projectile_SetBehaviorMode(u8 *,u16);
void Projectile_SetLaunchParameters(u8 *,u16,u32,u8,u32);
void Projectile_SetScriptProcedure(u8 *,u32,u16);
void Projectile_SetSpriteParameters(u8 *,u32,u32,u32);
void Projectile_SetTrackingTarget(u8 *,u32);
void Sound_PlaySong(u32);
void Projectile_InitParameters(u8 *);
int sub_081FD01C(u8 *,ProjectilePosition *);
static inline int trunc12(int n) { return n>=0?n>>12:-(-n>>12); }
static inline void sum(ProjectilePosition *dst, ProjectilePosition *a, ProjectilePosition *b) {
 dst->x=b->x+a->x; dst->y=b->y+a->y; dst->z=b->z+a->z;
}
void sub_081F6748(SpreadProjectileActor *p,ProjectilePosition *position,u32 parameter)
{
 u8 projectile[0x58];
 ProjectilePosition delta;
 ProjectilePosition spawn;
 u8 angle;
 u8 facing;
 u16 step;
 int speed;
 int rawFacing;
 int count;
 s16 *velocity;
 ProjectilePosition *target;
 ProjectilePosition *spawnPtr;
 target=&p->target;
 angle=(u8)Math_Atan2(target->x-position->x,target->z-position->z);
 step=16;
 facing=(p->facing<<6)-64;
 velocity=(s16 *)&delta;
 facing=(u8)facing;
 speed=180;
 { int x=trunc12(gUnk_086149C4[(facing+64)&255]*speed); int zero=0; velocity[0]=x; velocity[1]=zero; }
 velocity[2]=trunc12(gUnk_086149C4[facing]*speed);
 sum(&spawn,&delta,position);
 spawn.y+=200;
 angle=(u8)(angle-step);
 spawnPtr=&spawn;
 count=2;
 do {
 Projectile_InitParameters(projectile);
 Projectile_SetLaunchParameters(projectile,0,parameter,angle,24);
 Projectile_SetSpriteParameters(projectile,0x8e5a,0,498);
 Projectile_SetScriptProcedure(projectile,0x4363,0);
 Projectile_SetTrackingTarget(projectile,(u32)&p->target);
 Projectile_SetBehaviorMode(projectile,1);
 sub_081FD01C(projectile,spawnPtr);
 angle=(u8)(angle+step);
 --count;
 } while(count>=0);
 Sound_PlaySong(1384);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081F6748);
#endif
