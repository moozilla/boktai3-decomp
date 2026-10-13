#include "global.h"
#include "scene_object.h"
void sub_0822B2F8(s32);
static inline void setMode(struct SceneDraw *d, u16 mode)
{
    d->mode = mode;
}
void Scene_UpdateObjectFlash(struct SceneObject *p)
{
    if (p->entered) {
        p->entered = 0;
        p->phase = 3;
        setMode(&p->draw, 3);
        sub_0822B2F8(0x136);
    }
    if (p->phase) {
        if (p->timer++ > 5) {
            p->timer = 0;
            p->phase--;
            setMode(&p->draw, p->phase);
        }
    } else {
        register void (*callback)(void) asm("r0") = sub_080050AC;
        p->update = callback;
        p->timer = 0;
        p->entered = 1;
        p->kind = 0;
    }
}
