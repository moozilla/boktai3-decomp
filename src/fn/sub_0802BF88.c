#include "global.h"

struct S {
    u8 f[0x818];
    u8 a;
};
void sub_0821FE6C(u8 *);
void sub_080297E8(struct S *);
void SceneActors_Destroy(struct S *);
u32 sub_0802BF88(struct S *p)
{
    sub_0821FE6C(&p->a);
    sub_080297E8(p);
    SceneActors_Destroy(p);
    return 0;
}
