#include "global.h"
#include "ram.h"
void SceneMessages_UnregisterReceiver(u8 *);
void ParticlePool_Destroy(u32);
void GroundEffect_Destroy(u8 *);
s32 SceneNpc_OnDestroy(u8 *a, u8 *b)
{
    u8 *q = b + 0x20;
    SceneMessages_UnregisterReceiver(b + 0x168);
    ParticlePool_Destroy(*(u32 *)(q + 4));
    if (q[0xd] != 0)
        GroundEffect_Destroy(b + 0x344);
    gSceneNpc = 0;
}
