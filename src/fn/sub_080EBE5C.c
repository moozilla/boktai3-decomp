/* Tentative name based on the observed state transition or initialization. */
#include "global.h"
#include "scene_actor_reset.h"
union SceneActorCollisionVector {
    struct ContactPackedVector vector;
    u32 words[2];
};
void SceneActorCollision_InitBodyWithFlags(struct SceneActorRegistryResetView *, void *, void *, void (*)(void), void *, u32);
void SceneActorCollision_InitAttack(struct SceneActorRegistryResetView *, void *, void *, void (*)(void), u32, void *);
void Octopus_OnBodyContact(void);
void Octopus_OnAttackContact(void);
void SceneActorCollision_InitCompactBody(struct SceneActorRegistryResetView *actor)
{
    union SceneActorCollisionVector bounds, offset, attackBounds, attackOffset;
    bounds.vector.x = 64;
    bounds.vector.y = 256;
    bounds.vector.z = 64;
    offset.words[0] = 0x800000;
    offset.vector.z = 0;
    SceneActorCollision_InitBodyWithFlags(actor, &bounds, &offset, Octopus_OnBodyContact, actor, 0x5005);
    attackBounds.words[0] = 0;
    attackBounds.vector.z = 0;
    attackOffset.words[0] = 0;
    attackOffset.vector.z = 0;
    SceneActorCollision_InitAttack(actor, &attackBounds, &attackOffset, Octopus_OnAttackContact, 1, actor);
}
