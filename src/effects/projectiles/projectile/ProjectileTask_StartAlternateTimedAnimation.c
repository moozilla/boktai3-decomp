#include "global.h"
#include "effects/projectiles/projectile_event_query_layout.h"
void Projectile_SetUpdateCallback(void *, void *);
void ProjectileTask_OnAlternateTimedAnimation(void);
s32 ProjectileTask_StartAlternateTimedAnimation(struct ProjectileEventQuery *p)
{
    Projectile_SetUpdateCallback((u8 *)p + 0x18, ProjectileTask_OnAlternateTimedAnimation);
    (p + 5)->v = 0x19;
    return 0;
}
