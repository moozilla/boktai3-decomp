#include "global.h"
#include "effects/projectiles/projectile_event_query_layout.h"
void Projectile_SetUpdateCallback(void *, void *);
void Projectile_UpdateTimedRemoval(void);
s32 ProjectileTask_StartType4TimedAnimation(struct ProjectileEventQuery *p)
{
    Projectile_SetUpdateCallback((u8 *)p + 0x18, Projectile_UpdateTimedRemoval);
    (p + 5)->v = 0x19;
    return 0;
}
