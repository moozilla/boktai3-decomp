#include "global.h"
#include "effects/projectiles/projectile_event_query_layout.h"
void Projectile_SetUpdateCallback(void *, void *);
void ProjectileTask_OnDefaultTimedAnimation(void);
s32 ProjectileTask_StartDefaultTimedAnimation(struct ProjectileEventQuery *p)
{
    Projectile_SetUpdateCallback((u8 *)p + 0x18, ProjectileTask_OnDefaultTimedAnimation);
    (p + 5)->v = 0x19;
    return 0;
}
