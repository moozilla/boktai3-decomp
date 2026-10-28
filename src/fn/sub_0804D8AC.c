#include "global.h"
struct E {
    u8 a[6];
    u8 t;
};
struct L {
    u8 pad[6];
    u8 n[2];
    u32 e[2][4];
};
struct G {
    u8 pad[0x24];
    u8 idx;
};
extern struct G *gSceneEventDispatchState;
void SceneMessages_SetPendingTrigger(struct L *, struct E *);
s32 StagedEffect_CheckScriptCompletion(u8 *p)
{
    struct L *l = (struct L *)(p + 0x18);
    s32 i = 0;
    if (i < l->n[gSceneEventDispatchState->idx]) {
        do {
            struct G *g = gSceneEventDispatchState;
            struct E *e = (struct E *)l->e[g->idx][i];
            SceneMessages_SetPendingTrigger(l, e);
            if (e->t == 3)
                return 1;
            i++;
        } while (i < l->n[gSceneEventDispatchState->idx]);
    }
    return 0;
}
