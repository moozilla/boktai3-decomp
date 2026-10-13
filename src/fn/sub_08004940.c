#include "global.h"
#include "scene_object.h"
s32 sub_0822B2F8(u32);
void sub_08014CA4(u32, u32, struct SceneVec *, u32, u32, u32, u32, u32, u32, u32, u32, u32);

void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32, u32);

void Scene_PlayObjectBreakEffect(struct SceneObject *p)
{
    struct SceneExtent a;
    struct SceneExtent b;
    struct SceneExtent c;
    u32 d;
    struct SceneVec *q;
    sub_0822B2F8(0x15B);
    q = &p->body.position;
    d = 0x1e;
    sub_08014CA4(3, 2, q, 0x3c, d, 0x10, 8, 8, 0, 0x100, 0x18, 0x10);
    sub_08014CA4(8, 6, q, 0x3c, d, 0x16, 8, 8, 0, 0x100, 0x18, 0x10);
    a.x = p->body.position.x;
    a.y = p->body.position.y + 0x80;
    a.z = p->body.position.z;
    b.x = 0xFFF1;
    b.y = 8;
    b.z = 0xFFF1;
    c.x = d;
    c.y = 0x16;
    c.z = d;
    sub_08014634(8, 3, &a, &b, &c, 0x3c, 0x3c);
}
