#include "global.h"
#include "effects/projectiles/projectile_event_query_layout.h"
void Projectile_SetUpdateCallback(void *, void *);
void TimedEffect_TestDuration(void);
s32 Projectile_InitAlternatingExpiry(struct ProjectileEventQuery *p)
{
    Projectile_SetUpdateCallback((u8 *)p + 0x18, TimedEffect_TestDuration);
    (p + 5)->v = 0x19;
    return 0;
}
