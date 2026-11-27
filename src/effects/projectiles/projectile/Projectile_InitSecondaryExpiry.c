#include "global.h"
#include "effects/projectiles/projectile/projectile_event_query_layout.h"
void Projectile_SetUpdateCallback(void *, void *);
void Projectile_UpdateSecondaryExpiry(void);
s32 Projectile_InitSecondaryExpiry(struct ProjectileEventQuery *p)
{
    Projectile_SetUpdateCallback((u8 *)p + 0x18, Projectile_UpdateSecondaryExpiry);
    (p + 5)->v = 0x19;
    return 0;
}
