#include "global.h"

void Axe_RemoveSecondaryColliders(void);
void Axe_RemoveAuxiliarySprites(void *);
void Axe_ReleaseAuxiliarySprite(void *);
void SceneActorEffects_ReleaseResources(void *);

s32 Axe_ReleaseResources(void *p)
{
    Axe_RemoveSecondaryColliders();
    Axe_RemoveAuxiliarySprites(p);
    Axe_ReleaseAuxiliarySprite(p);
    SceneActorEffects_ReleaseResources(p);
    return 0;
}
