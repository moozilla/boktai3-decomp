#include "global.h"
struct Motion {
    u32 id, flags;
    u8 pad[0x40];
};
struct Object {
    u8 pad[0x1b];
    u8 kind;
    u8 pad1c[0x40];
    struct Motion motion;
    u8 vector[8];
};
void SceneBody_Init(void *, int, int, int, int, void *);
int SceneBody_SetPositionTarget(void *, void *);
int Actor_InitMotion(struct Object *p, int a, int b, int kind)
{
    struct Motion *m;
    int result;
    p->kind = kind;
    m = &p->motion;
    SceneBody_Init(m, a, b, (u8)kind, 7, p);
    result = SceneBody_SetPositionTarget(m, p->vector);
    {
        u32 mask = 4;
        m->flags |= mask;
    }
    return result;
}
