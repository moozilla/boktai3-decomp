#include "engine/scene_body/scene_body_api.h"
#include "engine/shared/scene_actor_components.h"

void CellObject_Init(void *object, const struct ContactPosition *position);

u32 SceneBody_SetMovementProbe(struct SceneActorBody *body, void *probe, u16 extentX, u16 extentZ)
{
    body->movementProbe = probe;
    CellObject_Init(probe, &body->position);
    body->probeExtentX = extentX;
    body->probeExtentZ = extentZ;
    return 1;
}
