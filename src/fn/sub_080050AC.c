#include "global.h"
#include "scene_object.h"

/* Idle state: wait until health runs out, then disappear (type 1) or get removed. */
void sub_080050AC(struct SceneObject *p)
{
    u8 t = p->type;
    if (t == 1) {
        if (p->health <= 0) {
            void *f = Scene_UpdateObjectDisappearance;
            p->update = f;
            p->timer = 0;
            p->entered = t;
            p->kind = 3;
        }
    } else {
        if (p->health <= 0) {
            p->mode = 3;
            p->update = 0;
        }
    }
}
