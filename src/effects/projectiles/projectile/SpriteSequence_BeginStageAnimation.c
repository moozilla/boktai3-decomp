#include "effects/projectiles/projectile_task.h"
void *ResourceRegistry_GetResource(u16);
void ProjectileTask_SetTrailSpriteResources(void *, u32);
void ProjectileTask_UpdateTrailImpact(void);
extern const u32 gUnk_08613684[];
s32 SpriteSequence_BeginStageAnimation(struct Task4E01Projectile *p)
{
    struct Task4E01Params *params = &p->params;
    void *group = ResourceRegistry_GetResource(0x4E01);
    s32 result;
    if (group) {
        p->runtime.trailFrame = 1;
        {
            struct ContactSwarmSprite *sprite = &p->sprite;
            sprite->frame = 0x1A;
        }
        Projectile_SetUpdateCallback(params, ProjectileTask_UpdateTrailImpact);
        ProjectileTask_SetTrailSpriteResources(group, gUnk_08613684[params->element]);
        result = 0;
    } else {
        result = -1;
    }
    return result;
}
