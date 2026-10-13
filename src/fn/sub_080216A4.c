#include "global.h"
#define SceneActors_SetDamageCallback sub_080216A4
void sub_0815F6F0(void *, int, void *);
void sub_08021450(void);
void SceneActors_SetDamageCallback(u8 *p, int a, void *callback, int defense)
{
    *(int *)(p + 0x16c) = a;
    sub_0815F6F0(p + 0x15c, a, sub_08021450);
    *(void **)(p + 0x170) = callback;
    *(int *)(p + 0x158) = defense;
}
