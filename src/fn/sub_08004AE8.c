#include "global.h"
#include "scene_object.h"
s32 sub_0822B2F8(u32);
void sub_08014CA4(u32, u32, struct SceneVec *, u32, u32, u32, u32, u32, u32, u32, u32, u32);

void sub_08004AE8(struct SceneObject *object)
{
    struct SceneVec *p;
    sub_0822B2F8(0x150);
    p = &object->body.position;
    sub_08014CA4(3, 1, p, 0x3c, 0x1e, 0x10, 8, 8, 0, 0x100, 0x18, 0x10);
    sub_08014CA4(8, 5, p, 0x3c, 0x1e, 0x16, 8, 8, 0, 0x100, 0x18, 0x10);
}
