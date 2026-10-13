#include "global.h"
#include "scene_object.h"

void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
void sub_0821A0C0(void *);
s32 sub_0821ABA8(u32, u32);

/* Create the scene-object task (or return the existing one). */
struct SceneObjectManager *sub_08005778(void *a)
{
    struct SceneObjectManager *p;
    if (gUnk_02000024 != 0)
        return gUnk_02000024;
    p = sub_08219FBC(8, 0x74);
    if (p != 0) {
        sub_0821A04C(p, (void (*)(void))SceneObjects_Update, (void (*)(void))sub_080056D0);
        p->count = sub_0821ABA8(0x6d, 8);
        if (sub_080056F0(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
