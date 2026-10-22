#include "global.h"
u8 *SceneActors_GetBySlot(s32);
struct S {
    u32 a;
    u8 pad[20];
};
struct A {
    u8 pad[0xd18];
    struct S s[4];
};
void sub_081F33CC(s32 index)
{
    struct A *a = (struct A *)SceneActors_GetBySlot(4);
    a->s[index].a = 0;
}