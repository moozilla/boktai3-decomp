#include "global.h"

void Actor_UpdateTargetFacing(void *, void *);

void SceneCharacter_FaceSceneTarget(void *a, void *b)
{
    Actor_UpdateTargetFacing(a, b);
}
