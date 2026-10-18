#include "global.h"
struct E {
    u8 f[0xde];
    s8 b;
    u8 a;
};
struct G {
    u8 f[0x1e0];
    u32 m;
};
void TerrainOverlay_QueueRemoveActor(void *);
void SceneBody_Unregister(void *);
void Memory_Clear(void *, u32);
void SceneTerrainProbe_DestroyEntry(struct G *g, struct E *e)
{
    u8 *p;
    s32 k;
    e->a = 0;
    p = (u8 *)&e->b;
    k = *(s8 *)p;
    g->m &= ~(1 << k);
    TerrainOverlay_QueueRemoveActor(e);
    SceneBody_Unregister(e);
    Memory_Clear(e, 0xe4);
    *p = 0xff;
}
