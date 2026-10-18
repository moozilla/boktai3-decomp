#include "global.h"
void TargetRegistry_RegisterSceneActor(void *, int, void *);
void SceneActors_ApplyDamage(void);
void SceneActors_SetDamageCallback(u8 *p, int a, void *callback, int defense)
{
    *(int *)(p + 0x16c) = a;
    TargetRegistry_RegisterSceneActor(p + 0x15c, a, SceneActors_ApplyDamage);
    *(void **)(p + 0x170) = callback;
    *(int *)(p + 0x158) = defense;
}
