#define Hresvelgr_CreateProjectileExplosions sub_081A4AB4
#define gHresvelgrProjectilePool gUnk_02000244
#define gRandomIndex gUnk_03005308
#define gRandomTable gUnk_0203B400
#include "global.h"
struct ProjectilePosition { s16 x,y,z,pad; };
struct HresvelgrProjectile { u8 unknown0[0x60]; struct ProjectilePosition origin; };
struct HresvelgrProjectilePool { u8 unknown0[0x5c]; s16 duration, effectParameter, effectHeight; };
extern struct HresvelgrProjectilePool *gHresvelgrProjectilePool;
extern const u16 gRandomTable[];
extern u32 gRandomIndex;
void *ExplosionEffect_CreateConfigured(void *,u32,s32,s32,s32,s32,s32,u32);
static inline int randomValue(const u16 *table)
{
 u32 offset;
 gRandomIndex = (gRandomIndex + 1) & 1023;
 offset = gRandomIndex * 2;
 return *(const u16 *)(offset + (u32)table);
}
void sub_081A4AB4(struct HresvelgrProjectile *p)
{
 struct ProjectilePosition position;
 struct HresvelgrProjectilePool *pool = gHresvelgrProjectilePool;
 int i = 0;
 struct ProjectilePosition *v = &position;
 for (; i < 8; ++i) {
 position = p->origin;
 if (i > 0) {
 u32 delta;
 delta = (randomValue(gRandomTable) >> 3) % 512 - 256;
 v->x = delta + v->x;
 delta = ((randomValue(gRandomTable) >> 3) & 512) - 256;
 v->z = v->z + delta;
 }
 ExplosionEffect_CreateConfigured(&position, 16, pool->duration, pool->effectParameter, 0x40000, pool->effectHeight, 0, i << 3);
 }
}
