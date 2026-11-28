#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

void Task_Kill(void *);
void ScaledShadow_Destroy(void *);
void Collider_Remove(void *);
u32 Ratatosk_RemoveAnimation(struct Ratatosk *);
void SceneActors_Destroy(struct Ratatosk *);

u32 Ratatosk_Destroy(struct Ratatosk *p)
{
    if (p->homingProjectiles)
        Task_Kill(p->homingProjectiles);
    if (p->playerClones)
        Task_Kill(p->playerClones);
    if (p->particles)
        Task_Kill(p->particles);
    if (p->animatedParticles)
        Task_Kill(p->animatedParticles);
    if (p->sceneProjectiles)
        Task_Kill(p->sceneProjectiles);
    if (p->sunOrbs)
        Task_Kill(p->sunOrbs);
    ScaledShadow_Destroy(p->shadow);
    Collider_Remove(&p->hurtCollider);
    Ratatosk_RemoveAnimation(p);
    SceneActors_Destroy(p);
    return 0;
}
