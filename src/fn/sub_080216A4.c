#include "global.h"
void sub_0815F6F0(void *, int, void *);
void SceneActors_ApplyDamage(void);
void SceneActors_SetDamageCallback(u8 *p, int a, void *callback, int defense)
{
    *(int *)(p + 0x16c) = a;
    sub_0815F6F0(p + 0x15c, a, SceneActors_ApplyDamage);
    *(void **)(p + 0x170) = callback;
    *(int *)(p + 0x158) = defense;
}
