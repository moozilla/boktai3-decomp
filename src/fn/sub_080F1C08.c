#include "global.h"

struct P {
    u8 f[0xcc5];
    u8 a;
    u8 b;
    u8 g[0xcf0 - 0xcc7];
    void (*cb)(void);
    u8 h[0xd32 - 0xcf4];
    u16 v;
};
void SceneActor_RampColorRed(void);
s32 Ivy_InitColorTransition(struct P *p)
{
    u16 *s = &p->v;
    p->a = (*s >> 5) & 0x1f;
    p->b = (*s >> 10) & 0x1f;
    p->cb = SceneActor_RampColorRed;
}