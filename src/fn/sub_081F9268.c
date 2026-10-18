#define SceneEffect_EnterAndRestart sub_081F9268
#include "global.h"
struct SceneEffectState {
    u8 pad[0xAA];
    u8 st;
    u8 b0;
    u8 b1;
    u8 b2;
    u8 b3;
    u8 f1;
    u8 f2;
    u8 pad3[0xC4 - 0xB1];
    u32 cnt;
};
void SceneActors_ConfigureEffect(u8 *, unsigned, int, int);
static inline u8 TakeFlag(struct SceneEffectState *p)
{
    if (p->f2) {
        p->f2 = 0;
        p->f1 = 0;
        return TRUE;
    }
    return FALSE;
}
void sub_081F9268(struct SceneEffectState *p)
{
    u32 *c;
    u8 t, entry;
    entry = TakeFlag(p);
    c = &p->cnt;
    if (entry) {
        SceneActors_ConfigureEffect((u8 *)p, 5, *c, 120);
    }
    t = p->f1;
    if (t) {
        u32 z = 0, o = 1;
        p->b1 = z;
        p->b2 = z;
        p->b3 = o;
        *c = z;
        p->f2 = o;
    }
    (*c)++;
}
