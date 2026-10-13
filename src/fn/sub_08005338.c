#include "global.h"
#include "scene_object.h"
struct Args {
    u32 count : 16;
    s32 *values;
};
void sub_0822B2F8(s32);
void sub_0821AD08(u32, struct Args *);
s32 Mod(s32, s32);
static inline void setMode(struct SceneDraw *draw, s16 mode)
{
    draw->mode = mode;
}
void Scene_UpdateObjectDisappearance(struct SceneObject *p)
{
    s32 values[5];
    struct Args args;
    if (p->entered) {
        p->entered = 0;
        p->phase = 0;
        setMode(&p->draw, 0);
        sub_0822B2F8(0x136);
        p->cellRef.cell.flags |= 0x21;
        if (p->script) {
            values[0] = p->body.id;
            values[1] = p->type;
            values[2] = p->body.position.x;
            values[3] = p->body.position.y;
            values[4] = p->body.position.z;
            args.count = 5;
            args.values = values;
            sub_0821AD08(p->script, &args);
            p->script = 0;
        }
    }
    if (!p->phase) {
        if (!Mod(p->timer, 2)) {
            if (!p->draw.mode)
                setMode(&p->draw, 1);
            else
                setMode(&p->draw, 0);
        }
        if (p->timer++ > 29) {
            p->timer = 0;
            p->phase++;
        }
    } else {
        if (p->timer++ > 5) {
            p->timer = 0;
            if (++p->phase > 3)
                p->mode = 2;
            else
                setMode(&p->draw, p->phase);
        }
    }
}
