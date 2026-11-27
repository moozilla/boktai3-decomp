#include "global.h"
void ParticlePool_Destroy(u32);
void SceneMessages_UnregisterReceiver(u8 *);
s32 SceneCharacter_OnDestroy(u8 *a, u8 *b)
{
    if (b[1] == 0x1d)
        ParticlePool_Destroy(*(u32 *)(b + 0x20));
    SceneMessages_UnregisterReceiver(b + 0x168);
}
